/*************************************************************************************************************************************************/
/*!     ADS1220.c
*
*       This code is designed to perform standard command and control operations on the ADS1220 over a SPI bus. Functions exist to setup, configure,
*       and read conversion data from the ADS1220.
*
*       The software is specifically written to execute on an MSP430G2413 on the SATxxxx board.
*
*       October 2013
*
*       \note that the functions in this file are not re-entrant. It is the user's responsibility to assure that these functions
*       are not called until the previous function has completed.
*/
/**************************************************************************************************************************************************
*       Copyright 锟�013 Texas Instruments Incorporated - http://www.ti.com/                                                                      *
***************************************************************************************************************************************************
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
*                                                                                                                                                 *
*    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.                 *
*                                                                                                                                                 *
*    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the        *
*    documentation and/or other materials provided with the distribution.                                                                         *
*                                                                                                                                                 *
*    Neither the name of Texas Instruments Incorporated nor the names of its contributors may be used to endorse or promote products derived      *
*    from this software without specific prior written permission.                                                                                *
*                                                                                                                                                 *
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          *
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     *
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         *
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    *
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE      *
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                                           *
**************************************************************************************************************************************************/
#include "def.h"
#include "Kernel.h"
#include "ADS1120.h"

static u8 SpiHandle;
static u8 RcvData[8];

static int OffsetCalibrateData = 0;
static u8 OffsetCalibrateCount = 0;
static int OffsetCalibrateValue = 0;

float PgaGainLevel;

int GainCalibrateData = 0;
u8 GainCalibrateCount = 0;
int GainCalibrateValue = 0;

int RawVoltage;
int CalVoltage;
int GainVoltage;

extern float FlashGainCorrection;

/*=================================================================================*/
#if 0
/*
GPE13 ---- SCLK
GPE12 ---- MOSI
GPE11 ---- MISO
GPF4  ---- CS
GPF3  ---- RDY
*/
#endif

//static	GPIO_REG *gpio = GPIO_REG_VA_BASE;
static volatile BOOL adc_irq=FALSE;
static sem_obj *sem_adc=NULL;

static u32 dummy_port=0;

#define	SCLK_PORT	dummy_port
#define	SCLK_BIT	13

#define	MOSI_PORT	dummy_port
#define	MOSI_BIT	12

#define	MISO_PORT	dummy_port
#define	MISO_BIT	11

#define	CS_PORT	    dummy_port
#define	CS_BIT		4

#define	RDY_PORT    dummy_port
#define	RDY_BIT		3

////////

#define	CS_0()	CS_PORT	&= ~(1<<CS_BIT)
#define	CS_1()	CS_PORT	|=  (1<<CS_BIT)

#define	MOSI_0()	MOSI_PORT &= ~(1<<MOSI_BIT)
#define	MOSI_1()	MOSI_PORT |=  (1<<MOSI_BIT)

#define	SCLK_0()	SCLK_PORT &= ~(1<<SCLK_BIT)
#define	SCLK_1()	SCLK_PORT |=  (1<<SCLK_BIT)

#define	MISO_1()	(MISO_PORT&(1<<MISO_BIT))

#define	RDY_1()		(RDY_PORT&(1<<RDY_BIT))


static volatile u16 adc_index=0;
static volatile u16 adc_count=10;
static volatile u16 adc_buf[256];

u16 ADS1120_ReadData (void);

static void adc_isr(void)
{

	if(adc_index<adc_count)
	{
		adc_buf[adc_index]=ADS1120_ReadData();

		adc_index++;
		if(adc_index >= adc_count)
		{	//AD闁插洦鐗遍弫鏉跨暚閹达拷
			SYS_sem_post(sem_adc);
		}
		else
		{	//閸氼垰濮╂稉瀣╃濞嗩搰D鏉烆剚宕�
			ADS1120_Start();
		}
	}
	EINT_ClearPend(11);
}

static void Port_Init(void)
{
	sem_adc =SYS_sem_create(0,1,NULL);
/*
	GPE13 ---- SCLK
	GPE12 ---- MOSI
	GPE11 ---- MISO
	GPF4  ---- CS
	GPF3  ---- RDY
*/
#if 0
	GPIO_SetMode(GPE,13,GPIO_PULL_UP|GPIO_OUTPUT); //SCLK
	GPIO_SetMode(GPE,12,GPIO_PULL_UP|GPIO_OUTPUT); //MOSI
	GPIO_SetMode(GPE,11,GPIO_PULL_UP|GPIO_INPUT);  //MISO
	GPIO_SetMode(GPF,4,GPIO_PULL_UP|GPIO_OUTPUT);  //CS
	GPIO_SetMode(GPF,3,GPIO_PULL_UP|GPIO_INPUT);   //RDY
#endif
	CS_1();
	SCLK_0();
	MOSI_1();


#if 0
	EINT_Enable(11,FALSE);
	EINT_SetPin(11,TRUE);
	EINT_SetMode(11,EINT_MODE_RISING);
	//EINT_SetMode(11,EINT_MODE_FALLING);
	EINT_ClearPend(11);
	EINT_SetHandler(11,adc_isr);
	EINT_Enable(11,TRUE);
#endif

	//while(1)
	{
		Sleep(200);
	}

}

static void delay(void)
{
	volatile int i;
	for(i=0;i<1;i++);
}

static void SPI_CS_0 (void)
{
	CS_0();
}

static void SPI_CS_1(void)
{
	CS_1();
}

static void SPI_WriteByte(u8 dat)
{
//	GPIO_REG *gpio =GPIO_REG_VA_BASE;
	int  i;

	for(i = 0; i < 8;i++)
    {
	    if(dat&0x80)
	    {
	       MOSI_1();
	    }
	    else
	    {
	       MOSI_0();
	    }

	    dat <<= 1;
	    delay();
	    SCLK_1();
	    delay();
	    SCLK_0();
	    delay();
	 }

}
static u8 SPI_ReadByte(void)
{
	   u8 dat,i;

	    for(i=0; i<8; i++)
	    {
	    	SCLK_1();
	    	delay();
	    	SCLK_0();
	    	delay();

	    	dat <<= 1;
	        if(MISO_1())
	        {
	        	dat |= 0X01;
	        }

	    }
	    return dat;

}
/*
static u8 SPI_ReadByte(void)
{
	   u8 dat,i;

	   SCLK_0();
	   delay();
	    for(i = 0;i<8; i++)
	    {
	    	SCLK_1();
	    	delay();
	    	dat <<= 1;
	        if(MISO_1())
	        {
	        	dat |= 0X01;
	        }
	        SCLK_0();
	        delay();
	    }
	    return dat;

}
*/
#if 0
static u8 SPI_RW_Byte(u8 dat)
{
	int i;
	GPIO_REG *gpio =GPIO_REG_VA_BASE;

   	for(i=0;i<8;i++)   // output 8-bit
   	{
   		if(dat&0x80)
   		{
   			MOSI_1();
   		}
   		else
   		{
   			MOSI_0();
   		}

   		dat = (dat << 1);           // shift next bit into MSB..
   		SCLK_1();

   		if(MISO_1())
   		{
   			dat |= 1;
   		}
   		SCLK_0();            		  // ..then set SCK low again
   		//spi_delay();
   	}

    return (dat);           		  // return read byte
}
#endif

static void SPI_Write(u8 *buf,int count)
{
	int i;
	for(i=0;i<count;i++)
	{
		SPI_WriteByte(buf[i]);
	}
}

static void SPI_Read(u8 *buf,int count)
{
	int i;
	for(i=0;i<count;i++)
	{
		buf[i]=SPI_ReadByte();
	}

}


/*=================================================================================*/

BOOL ADS1220_WaitRdy(void)
{
	return TRUE;
	while(RDY_1());
	return TRUE;
}

/*************************************************************************************************************************************************
*  Setup_ADS1220
**************************************************************************************************************************************************/
/*!
* @brief Performs the setup of the ADS1220.
*
* This function will configure the ADS1220.
*
* @param[in]     ste_port        The MSP430 port that contains the CS or STE pin that is connected to the ADS1220. (PORT_1, PORT_2, ...)
* @param[in]     ste_pin         The MSP430 pin that contains the CS or STE pin that is connected to the ADS1220.  (BIT0, BIT1, ...)
* @param[in]     inputMux        Mux settings for the AIN pins (ADS1220_MUX_AIN0_AIN1, ADS1220_MUX_AIN0_AIN2, ADS1220_MUX_AIN0_AIN3,
*                                                               ADS1220_MUX_AIN1_AIN2, ADS1220_MUX_AIN1_AIN3, ADS1220_MUX_AIN2_AIN3,
*                                                               ADS1220_MUX_AIN1_AIN0, ADS1220_MUX_AIN3_AIN2, ADS1220_MUX_AIN0_AVSS,
*                                                               ADS1220_MUX_AIN1_AVSS, ADS1220_MUX_AIN2_AVSS, ADS1220_MUX_AIN3_AVSS,
*                                                               ADS1220_MUX_REFP-REFN, ADS1220_MUX_AVDD-AVSS, ADS1220_MUX_SHORTED)
* @param[in]     opMode          ADS1220 operating mode (ADS1220_OP_MODE_NORMAL, ADS1220_OP_MODE_DUTY, ADS1220_OP_MODE_TURBO)
* @param[in]     conversionMode  Identifies single shot or continuous conversions (ADS1220_CONVERSION_SINGLE_SHOT or ADS1220_CONVERSION_CONTINUOUS)
* @param[in]     dataRate        ADS1220 Data Rate (ADS1220_DATA_RATE_20SPS, ADS1220_DATA_RATE_45SPS, ADS1220_DATA_RATE_90SPS,
*                                                   ADS1220_DATA_RATE_175SPS, ADS1220_DATA_RATE_330SPS, ADS1220_DATA_RATE_600SPS, or
*                                                   ADS1220_DATA_RATE_1000SPS)
* @param[in]     gainLevel       ADS1220 Gain Level (ADS1220_GAIN_1, ADS1220_GAIN_2, ADS1220_GAIN_4, ADS1220_GAIN_8, ADS1220_GAIN_16,
*                                                    ADS1220_GAIN_32, ADS1220_GAIN_64, or ADS1220_GAIN_128)
* @param[in]     pgaBypass       Bypass PGA for gain settings - Note the PGA will be used whenever the gain setting is ADS1220_GAIN_8 or greater,
*                                no matter what this value is set to. (ADS1220_PGA_BYPASS or ADS1220_USE_PGA)
* @param[in]     routeIDAC1      The output pin used by IDAC1 (ADS1220_IDAC1_DISABLED, ADS1220_IDAC1_AIN0, ADS1220_IDAC1_AIN1, ADS1220_IDAC1_AIN2,
*                                                              ADS1220_IDAC1_AIN3, ADS1220_IDAC1_REFP, or ADS1220_IDAC1_REFN
* @param[in]     routeIDAC2      The output pin used by IDAC2 (ADS1220_IDAC2_DISABLED, ADS1220_IDAC2_AIN0, ADS1220_IDAC2_AIN1, ADS1220_IDAC2_AIN2,
*                                                              ADS1220_IDAC2_AIN3, ADS1220_IDAC2_REFP, or ADS1220_IDAC2_REFN
* @param[in]     idacCurrent     The IDAC Biasing Current (ADS1220_IDAC_CURRENT_OFF, ADS1220_IDAC_CURRENT_10_UA, ADS1220_IDAC_CURRENT_50_UA,
*                                                          ADS1220_IDAC_CURRENT_100_UA, ADS1220_IDAC_CURRENT_250_UA, ADS1220_IDAC_CURRENT_500_UA,
*                                                          ADS1220_IDAC_CURRENT_1000_UA, ADS1220_IDAC_CURRENT_1500_UA
*
* @return  None
*
* @sa Setup_ADS1220_CS()
**************************************************************************************************************************************************/
void Setup_ADS1220 (u8 inputMux, u8 opMode,
                    u8 conversionMode, u8 dataRate, u8 gainLevel, u8 pgaBypass,
                    u8 routeIDAC1, u8 routeIDAC2, u8 idacCurrent)
{
u8 config[4];

    config[0] = inputMux + gainLevel + pgaBypass;
    config[1] = dataRate + opMode + conversionMode + ADS1220_TEMP_SENSOR_OFF + ADS1220_BURN_OUT_CURRENT_OFF;
    config[2] = ADS1220_FIR_50_60 + ADS1220_VREF_EXT_REF0_PINS + ADS1220_LOW_SIDE_POWER_OPEN + idacCurrent;
    config[3] = routeIDAC1 + routeIDAC2 + ADS1220_DRDY_ON_DRDY_ONLY;

    switch (gainLevel)
    {
        case ADS1220_GAIN_1:
            PgaGainLevel = 1.0;
            break;
        case ADS1220_GAIN_2:
            PgaGainLevel = 2.0;
            break;
        case ADS1220_GAIN_4:
            PgaGainLevel = 4.0;
            break;
        case ADS1220_GAIN_8:
            PgaGainLevel = 8.0;
            break;
        case ADS1220_GAIN_16:
            PgaGainLevel = 16.0;
            break;
        case ADS1220_GAIN_32:
            PgaGainLevel = 32.0;
            break;
        case ADS1220_GAIN_64:
            PgaGainLevel = 64.0;
            break;
        case ADS1220_GAIN_128:
            PgaGainLevel = 128.0;
            break;

    }
    ADS1220_Write_Regs (config, ADS1220_CONFIG_0_REG, 4);
}



/*************************************************************************************************************************************************
*  ADS1220_Offset_Calibrate_Data
**************************************************************************************************************************************************/
/*!
* @brief Performs a calibration step from data collected by the ADS1220.
*
* In order to perform a calibration, the ADS1220 AIN lines are shorted together (using the Setup_ADS1220() function). A number of conversion are
* then performed and the conversion data is sent to this function.
*
* This function will take the conversion data and average the results which will be used in the ADS1220_Get_Conversion_Data_Calibrated() function
* to return a calibrated conversion result.
*
* @param[in]    *tempData        Pointer to raw conversion data that will be included in calibration calculations.
*
* @return  None
*
* @sa Setup_ADS1220()
* @sa ADS1220_Get_Conversion_Data_Calibrated()
**************************************************************************************************************************************************/
void ADS1220_Offset_Calibrate_Data (u8 *tempData)
{
    int temp;

    temp = tempData[0];
    temp <<= 8;
    temp |= tempData[1];
    temp <<= 8;
    temp |= tempData[2];

    // Was temp negative?
    if (tempData[0] & 0x80)
        temp |= ((int)0xff << 24);              // Sign extend

    OffsetCalibrateData += temp;
    OffsetCalibrateCount++;
    OffsetCalibrateValue = OffsetCalibrateData / OffsetCalibrateCount;

}

void ADS1220_Gain_Calibrate_Data (u8 *tempData)
{
    int temp;

    temp = tempData[0];
    temp <<= 8;
    temp |= tempData[1];
    temp <<= 8;
    temp |= tempData[2];

    // Was temp negative?
    if (tempData[0] & 0x80)
        temp |= ((int)0xff << 24);              // Sign extend

    GainCalibrateData += temp;
    GainCalibrateCount++;
    GainCalibrateValue = GainCalibrateData / GainCalibrateCount;


    // To calculate fraction, take the expected answer from the precision multimeter reading and
    //   Code = RTDmeas * PgaGainLevel * (2^23 - 1) / (Rref * 2)

}


/*************************************************************************************************************************************************
*  ADS1220_Write_Regs
**************************************************************************************************************************************************/
/*!
* @brief Writes registers on the ADS1220.
*
* This function will execute a write register command to the ADS1220. This function can be used to update one or more registers on the ADS1220.
* No error checking is performed, so it is the user's responsibility to make sure they do not attempt to write past the end of the ADS1220 registers.
*
* @param[out]   *writeValues    Pointer to the list of 8 bit register values to place in the ADS1220
* @param[in]     startReg       Address of the first register to write
* @param[in]     length         Number of registers to write.
*
* @return  None
*
**************************************************************************************************************************************************/
void ADS1220_Write_Regs (u8 *writeValues, u8 startReg, u8 length)
{
    u8 buf[5];
    u8 i;

    SPI_CS_0();
    buf[0] = ADS1220_WRITE_CMD(startReg,length);

    for (i=0; i<length; i++)
    {
    	buf[i+1] = writeValues[i];
    }

    //Change_SPI_Phase(SPI_CHANGE_ON_FIRST_EDGE);
    SPI_Write (buf, length+1);    // Add 1 to length for command byte
    SPI_CS_1();
}

/*************************************************************************************************************************************************
*  ADS1220_Read_Regs
**************************************************************************************************************************************************/
/*!
* @brief Reads registers on the ADS1220.
*
* This function will execute a read register command to the ADS1220 and return the resultant data. This function can be used to read one or more
* registers from the ADS1220. No error checking is performed, so it is the user's responsibility to make sure they do not attempt to read past
* the end of the ADS1220 registers.
*
* @param[out]   *readValues     Pointer to place the 8 bit register values from the ADS1220
* @param[in]     startReg       Address of the first register to read
* @param[in]     length         Number of registers to read.
*
* @return  None
*
**************************************************************************************************************************************************/
#if 0
void ADS1220_Read_Regs (u8 *readValues, u8 startReg, u8 length)
{
    u8 buf[5] = {0x55, 0x55, 0x55, 0x55, 0x55};

    buf[0] = ADS1220_READ_CMD(startReg,length);

    //Change_SPI_Phase(SPI_CHANGE_ON_FIRST_EDGE);
   // SPI_Read (readValues, length+1);    // Add 1 to length for command byte

}
#endif

/*************************************************************************************************************************************************
*  ADS1220_Send_Read_Data_Command
**************************************************************************************************************************************************/
/*!
* @brief Sends a Read Data Command to the ADS1220.
*
* This function sends a Read Data (RDATA) command to the ADS1220 on the SPI bus.
*
* @return  None
*
**************************************************************************************************************************************************/
void ADS1220_Send_Read_Data_Command (void)
{
    u8 cmd = ADS1220_RDATA_CMD;
    //Change_SPI_Phase(SPI_CHANGE_ON_FIRST_EDGE);
    SPI_Write (&cmd, 1);
}



/*************************************************************************************************************************************************
*  ADS1220_Get_Conversion_Data_Calibrated
**************************************************************************************************************************************************/
/*!
* @brief Gets the conversion data from the ADS1220 and subtracts the calibration values to return a calibrated conversion data.
*
* This function gets the Conversion Data from the ADS1220. It will then subtract the Calibration Value previously calculated. This function
* is the standard function used to gather the conversion data during normal operations.
*
* The function requires the system to have performed calibration, otherwise the calibration value will be 0.
*
* @param[out]   *conversionData     Pointer to place the 24 bit Calibrated Conversion Data from the ADS1220
*
* @return  None
*
* @note The ADS1220_Get_Conversion_Data() function is used to return raw conversion data, which is used during calibration logic
*
* @sa ADS1220_Get_Conversion_Data()
**************************************************************************************************************************************************/
void ADS1220_Get_Conversion_Data_Calibrated (u8 *conversionData)
{
    u8 outData[3] = {0xff, 0xff, 0xff};
    u8 tempData[3];
    int temp_voltage;

    //Change_SPI_Phase(SPI_CHANGE_ON_FIRST_EDGE);
    SPI_Read (tempData, 3);    // 3 Bytes of Conversion Data

    temp_voltage = (((int)tempData[0] << 16) + ((int)tempData[1] << 8) + (int)tempData[2]);

    RawVoltage = temp_voltage;

    temp_voltage = temp_voltage - OffsetCalibrateValue;

    CalVoltage = temp_voltage;

    // Make sure that there is a value in place for the Gain
    if (*(unsigned int *)&FlashGainCorrection != 0xffffffff)
        temp_voltage = (int) ((float)temp_voltage * FlashGainCorrection);

    GainVoltage = temp_voltage;

    conversionData[0] = (temp_voltage >> 16) & 0xff;
    conversionData[1] = (temp_voltage >> 8) & 0xff;
    conversionData[2] = temp_voltage & 0xff;
}

/*============================================================================*/

void ADS1120_Reset(void)
{
	SPI_CS_0();
	SPI_WriteByte(ADS1220_RESET_CMD);
	SPI_CS_1();
}

void ADS1220_PowerDown(void)
{
	SPI_CS_0();
	SPI_WriteByte(ADS1220_POWERDOWN_CMD);
	SPI_CS_1();
}

/*************************************************************************************************************************************************
*  Setup_ADS1220
**************************************************************************************************************************************************/
/*!
* @brief Performs the setup of the ADS1220.
*
* This function will configure the ADS1220.
*
* @param[in]     ste_port        The MSP430 port that contains the CS or STE pin that is connected to the ADS1220. (PORT_1, PORT_2, ...)
* @param[in]     ste_pin         The MSP430 pin that contains the CS or STE pin that is connected to the ADS1220.  (BIT0, BIT1, ...)
* @param[in]     inputMux        Mux settings for the AIN pins (ADS1220_MUX_AIN0_AIN1, ADS1220_MUX_AIN0_AIN2, ADS1220_MUX_AIN0_AIN3,
*                                                               ADS1220_MUX_AIN1_AIN2, ADS1220_MUX_AIN1_AIN3, ADS1220_MUX_AIN2_AIN3,
*                                                               ADS1220_MUX_AIN1_AIN0, ADS1220_MUX_AIN3_AIN2, ADS1220_MUX_AIN0_AVSS,
*                                                               ADS1220_MUX_AIN1_AVSS, ADS1220_MUX_AIN2_AVSS, ADS1220_MUX_AIN3_AVSS,
*                                                               ADS1220_MUX_REFP-REFN, ADS1220_MUX_AVDD-AVSS, ADS1220_MUX_SHORTED)
* @param[in]     opMode          ADS1220 operating mode (ADS1220_OP_MODE_NORMAL, ADS1220_OP_MODE_DUTY, ADS1220_OP_MODE_TURBO)
* @param[in]     conversionMode  Identifies single shot or continuous conversions (ADS1220_CONVERSION_SINGLE_SHOT or ADS1220_CONVERSION_CONTINUOUS)
* @param[in]     dataRate        ADS1220 Data Rate (ADS1220_DATA_RATE_20SPS, ADS1220_DATA_RATE_45SPS, ADS1220_DATA_RATE_90SPS,
*                                                   ADS1220_DATA_RATE_175SPS, ADS1220_DATA_RATE_330SPS, ADS1220_DATA_RATE_600SPS, or
*                                                   ADS1220_DATA_RATE_1000SPS)
* @param[in]     gainLevel       ADS1220 Gain Level (ADS1220_GAIN_1, ADS1220_GAIN_2, ADS1220_GAIN_4, ADS1220_GAIN_8, ADS1220_GAIN_16,
*                                                    ADS1220_GAIN_32, ADS1220_GAIN_64, or ADS1220_GAIN_128)
* @param[in]     pgaBypass       Bypass PGA for gain settings - Note the PGA will be used whenever the gain setting is ADS1220_GAIN_8 or greater,
*                                no matter what this value is set to. (ADS1220_PGA_BYPASS or ADS1220_USE_PGA)
* @param[in]     routeIDAC1      The output pin used by IDAC1 (ADS1220_IDAC1_DISABLED, ADS1220_IDAC1_AIN0, ADS1220_IDAC1_AIN1, ADS1220_IDAC1_AIN2,
*                                                              ADS1220_IDAC1_AIN3, ADS1220_IDAC1_REFP, or ADS1220_IDAC1_REFN
* @param[in]     routeIDAC2      The output pin used by IDAC2 (ADS1220_IDAC2_DISABLED, ADS1220_IDAC2_AIN0, ADS1220_IDAC2_AIN1, ADS1220_IDAC2_AIN2,
*                                                              ADS1220_IDAC2_AIN3, ADS1220_IDAC2_REFP, or ADS1220_IDAC2_REFN
* @param[in]     idacCurrent     The IDAC Biasing Current (ADS1220_IDAC_CURRENT_OFF, ADS1220_IDAC_CURRENT_10_UA, ADS1220_IDAC_CURRENT_50_UA,
*                                                          ADS1220_IDAC_CURRENT_100_UA, ADS1220_IDAC_CURRENT_250_UA, ADS1220_IDAC_CURRENT_500_UA,
*                                                          ADS1220_IDAC_CURRENT_1000_UA, ADS1220_IDAC_CURRENT_1500_UA
*
* @return  None
*
* @sa Setup_ADS1220_CS()
**************************************************************************************************************************************************/
void ADS1120_Config (u8 inputMux, u8 opMode,
                    u8 conversionMode, u8 dataRate, u8 gainLevel, u8 pgaBypass,
                    u8 routeIDAC1, u8 routeIDAC2, u8 idacCurrent)
{
	u8 config[4];

    config[0] = inputMux + gainLevel + pgaBypass;
    config[1] = dataRate + opMode + conversionMode + ADS1220_TEMP_SENSOR_OFF + ADS1220_BURN_OUT_CURRENT_OFF;
    config[2] = ADS1220_FIR_50_60 + ADS1220_VREF_INTERNAL + ADS1220_LOW_SIDE_POWER_OPEN + idacCurrent;
    config[3] = routeIDAC1 + routeIDAC2 + ADS1220_DRDY_ON_DRDY_ONLY;

    switch (gainLevel)
    {
        case ADS1220_GAIN_1:
            PgaGainLevel = 1.0;
            break;
        case ADS1220_GAIN_2:
            PgaGainLevel = 2.0;
            break;
        case ADS1220_GAIN_4:
            PgaGainLevel = 4.0;
            break;
        case ADS1220_GAIN_8:
            PgaGainLevel = 8.0;
            break;
        case ADS1220_GAIN_16:
            PgaGainLevel = 16.0;
            break;
        case ADS1220_GAIN_32:
            PgaGainLevel = 32.0;
            break;
        case ADS1220_GAIN_64:
            PgaGainLevel = 64.0;
            break;
        case ADS1220_GAIN_128:
            PgaGainLevel = 128.0;
            break;

    }
    ADS1220_Write_Regs (config, ADS1220_CONFIG_0_REG, 4);
}

void CofigAD(u8 channel)
{
	u8 Init_Config[4];

    switch(channel)
    {
    case 0: Init_Config[0] = 0x81;
            break;
    case 1: Init_Config[0] = 0x91;
            break;
    case 2: Init_Config[0] = 0xA1;
            break;
    case 3: Init_Config[0] = 0xB1;
            break;
    }
    Init_Config[1] = 0X00;//Init_Config[1] = 0XC0;
    Init_Config[2] = 0X50;//Init_Config[2] = 0X40;
    Init_Config[3] = 0X00;
    ADS1220_Write_Regs(Init_Config,0,4);
   // ReadRegister();
}

void ADS1120_Start(void)
{
	SPI_CS_0();
	SPI_WriteByte(ADS1220_START_CMD);
	SPI_CS_1();
}

u16 ADS1120_ReadData(void)
{
    u8 a,b;

    SPI_CS_0();
    SPI_WriteByte(ADS1220_RDATA_CMD);
    a =SPI_ReadByte();
    b =SPI_ReadByte();
    SPI_CS_1();
    return (a<<8)|b;

}

/*============================================================================*/

void ADS1120_Init(void)
{
	Port_Init();
	ADS1120_Reset();
}

u16 ADS1120_GetData(u32 ch,u32 pga_gain)
{
	int a;


	ADS1120_Config(	ch,ADS1220_OP_MODE_NORMAL,
					ADS1220_CONVERSION_SINGLE_SHOT,ADS1220_DATA_RATE_1000SPS,pga_gain,ADS1220_USE_PGA,
					ADS1220_IDAC1_DISABLED,ADS1220_IDAC2_DISABLED,ADS1220_IDAC_CURRENT_OFF);
	ADS1120_Start();
	ADS1220_WaitRdy();
	a=ADS1120_ReadData();
	return a;
}


float ADS1120_GetVoltage_mV(u16 addata)
{
	float val;

	val=(float)addata*(float)2048/32768;
	return val;

}
/*============================================================================*/


void ADS1120_Test(void)
{

}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
