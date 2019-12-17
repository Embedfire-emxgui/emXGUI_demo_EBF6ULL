#include	"Include.h"
#include	"BSP.h"

/*=========================================================================================*/
/*=========================================================================================*/

const	char TargetBoard_IO[]={

"------------------------\r\n"
"iTOP-iMX6UL Target Board IO:\r\n"

"-------\r\n"

"\r\n"
};

/*=========================================================================================*/

void sysprintf(char *fmt,...)
{

}


/*=========================================================================================*/

uint32_t GetUartSrcClock(void)
{
    volatile uint32_t freq;

    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else
    {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}

////////

#define	UART_PIN_CFG	0x01B0B0u

void	UART0_PortInit(void)
{
	// GPE0;TXD0, GPE1:RXD0
	//reg32(REG_SYS_GPE_MFPL) &= ~((0xF<<4)|(0xF<<0));
	//reg32(REG_SYS_GPE_MFPL) |=  ((0x9<<4)|(0x9<<0));

}

void	UART1_PortInit(void)
{
	IOMUXC_MUX_UART->TXD1.U =0;
	IOMUXC_CFG_UART->TXD1.U =UART_PIN_CFG;

	IOMUXC_MUX_UART->RXD1.U =0;
	IOMUXC_CFG_UART->RXD1.U =UART_PIN_CFG;
}

void	UART2_PortInit(void)
{
	IOMUXC_MUX_UART->TXD2.U =0;
	IOMUXC_CFG_UART->TXD2.U =UART_PIN_CFG;

	IOMUXC_MUX_UART->RXD2.U =0;
	IOMUXC_CFG_UART->RXD2.U =UART_PIN_CFG;
}

void	UART3_PortInit(void)
{
	IOMUXC_MUX_UART->TXD3.U =0;
	IOMUXC_CFG_UART->TXD3.U =UART_PIN_CFG;

	IOMUXC_MUX_UART->RXD3.U =0;
	IOMUXC_CFG_UART->RXD3.U =UART_PIN_CFG;
}

void	UART4_PortInit(void)
{
	IOMUXC_MUX_UART->TXD4.U =0;
	IOMUXC_CFG_UART->TXD4.U =UART_PIN_CFG;

	IOMUXC_MUX_UART->RXD4.U =0;
	IOMUXC_CFG_UART->RXD4.U =UART_PIN_CFG;
}

void	UART5_PortInit(void)
{
	IOMUXC_MUX_UART->TXD5.U =0;
	IOMUXC_CFG_UART->TXD5.U =UART_PIN_CFG;

	IOMUXC_MUX_UART->RXD5.U =0;
	IOMUXC_CFG_UART->RXD5.U =UART_PIN_CFG;
}


/*=========================================================================================*/

static volatile int ii;
void led_test(int t)
{
	//volatile int i;
	gpio_pin_config_t cfg;


	IOMUXC_MUX_SNVS->TAMPER3.U =5;
	IOMUXC_CFG_SNVS->TAMPER3.U =0x10B0;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	GPIO_PinInit(GPIO5,3,&cfg);
	while(t-- > 0)
	{
		GPIO_PinWrite(GPIO5,3,1);
		for(ii=0;ii<0x7FFFFF;ii++);
		GPIO_PinWrite(GPIO5,3,0);
		for(ii=0;ii<0x7FFFFF;ii++);

	}

}

void	LED_Init(void)
{
	gpio_pin_config_t cfg;

	IOMUXC_MUX_SNVS->TAMPER3.U =5;
	IOMUXC_CFG_SNVS->TAMPER3.U =0x10B0;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	GPIO_PinInit(GPIO5,3,&cfg);

}

void	LED_ON(void)
{
	GPIO_PinWrite(GPIO5,3,1);
}

void	LED_OFF(void)
{
	GPIO_PinWrite(GPIO5,3,0);
}

/*=========================================================================================*/

#include "fsl_pwm.h"


void BL_PWM_Set(u8 val)
{
	PWM_Type *PWM =PWM1;
	pwm_config_t pwmConfig;
	u32 freq,duty,period;
	u32 clkIPG;

	clkIPG =CLOCK_GetFreq(kCLOCK_IpgClk);
	//// PWM1 for GPIO1_08
	//rIOMUXC_MUX_CFG(GPIO1_IO08,0,0x10B0);
	IOMUXC_MUX_GPIO1->IO08.U =0; //Mux=PWM1OUT.
	IOMUXC_CFG_GPIO1->IO08.U =0x10B0;
	//IOMUXC_Config(IOMUXC_GPIO1_IO08_PWM1OUT,0x10B0);

    /*!
      * config->enableStopMode = false;
      * config->enableDozeMode = false;
      * config->enableWaitMode = false;
      * config->enableDebugMode = false;
      * config->clockSource = kPWM_LowFrequencyClock;
      * config->prescale = 0U;
      * config->outputConfig = kPWM_SetAtRolloverAndClearAtcomparison;
      * config->fifoWater = kPWM_FIFOWaterMark_2;
      * config->sampleRepeat = kPWM_EachSampleOnce;
      * config->byteSwap = kPWM_ByteNoSwap;
      * config->halfWordSwap = kPWM_HalfWordNoSwap;
      */
     PWM_GetDefaultConfig(&pwmConfig);
     pwmConfig.prescale = 1000;
     pwmConfig.clockSource = kPWM_LowFrequencyClock;  //EXT 32KHZ
     //pwmConfig.clockSource = kPWM_HighFrequencyClock; //EXT 24MHZ

     PWM_Init(PWM, &pwmConfig);

     freq =1000; //1000HZ

     period =(clkIPG/(freq*pwmConfig.prescale))-2;//1000-2;
     //duty =2000;
     duty =(val*period)/100;

     PWM_SetSampleValue(PWM, duty);

     //PWMO (Hz) = PCLK(Hz) / (period +2)
     PWM_SetPeriodValue(PWM, period);
     PWM_StartTimer(PWM);
}

/*=========================================================================================*/

void Board_LowLevelInit(void)
{
	gpio_pin_config_t cfg;

	//// nPERI_PWREN
	//rIOMUXC_MUX(MUX_SNVS_TAMPER2).U =0x05;
	//rIOMUXC_CFG(CFG_SNVS_TAMPER2).U =0x10B0;
	IOMUXC_MUX_SNVS->TAMPER2.U =0x05;
	IOMUXC_CFG_SNVS->TAMPER2.U =0x10B0;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	UART1_PortInit();

	GPIO_PinInit(GPIO5,2,&cfg);
	GPIO_PinWrite(GPIO5,2,0);

	////
	LED_Init();
	led_test(3);
}

/*=========================================================================================*/

BOOL	PlaySound(const char *file_name,U32 flag)
{
	return FALSE;
}

/*=========================================================================================*/
