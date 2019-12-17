

#include <stdio.h>
#include <string.h>

#include 	"x_libc.h"
#include	"GUI_Drv.h"
#include	"BSP.h"
#include 	"GT9xx.h"


/*===================================================================================*/

#define	GT9xx_IIC_SPEED		IIC_SPEED_400KHZ

static	const struct i2c_ops *pIIC =&I2C_P1_28_P1_29;

#define CTP_RST_GPIO	GPIO3
#define CTP_RST_PIN		4

#define CTP_INT_GPIO 	GPIO5
#define CTP_INT_PIN 	9

//#define TOUCH_PAD_INT_IRQ 		GPIO1_Combined_0_15_IRQn
//#define TOUCH_PAD_IRQHANDLER    GPIO1_Combined_0_15_IRQHandler

#define	RST_0()	GPIO_PinWrite(CTP_RST_GPIO,CTP_RST_PIN,0)
#define	RST_1()	GPIO_PinWrite(CTP_RST_GPIO,CTP_RST_PIN,1)

#define	INT_0()	GPIO_PinWrite(CTP_INT_GPIO,CTP_INT_PIN,0)
#define	INT_1()	GPIO_PinWrite(CTP_INT_GPIO,CTP_INT_PIN,1)


#if 0
#define	GT_WR_ADDR (0xBA)
#define GT_RD_ADDR (0xBB)
#else
#define	GT_WR_ADDR (0x28)
#define GT_RD_ADDR (0x29)
#endif

static void CTP_RST_IOMUXC_INIT(void)
{
	IOMUXC_MUX_LCD->RESET.U =5; //GPIO3.4
	IOMUXC_CFG_LCD->RESET.U =0x10B0u;
}

static void CTP_INT_IOMUXC_INIT(void)
{
	IOMUXC_MUX_SNVS->TAMPER9.U =5; //GPIO5.9
	IOMUXC_CFG_SNVS->TAMPER9.U =0x10B0u;
}

static void GTP_IRQDisable(void)
{
	//DisableIRQ(TOUCH_PAD_INT_IRQ);
}


static void GTP_IRQEnable(void)
{

}

static void GTP_PortInit(void)
{
	gpio_pin_config_t cfg;

	CTP_RST_IOMUXC_INIT();
	CTP_INT_IOMUXC_INIT();


	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	//RST Pin Output Mode.
	cfg.direction =kGPIO_DigitalOutput;
	GPIO_PinInit(CTP_RST_GPIO,CTP_RST_PIN,&cfg);

	//INT Pin Output Mode.
	cfg.direction =kGPIO_DigitalOutput;
	GPIO_PinInit(CTP_INT_GPIO,CTP_INT_PIN,&cfg);


	//GTP_IRQEnable();
	////////
	INT_0();      //INT=0;
	RST_0();      //RST=0;

#if GT_WR_ADDR==0x28
	GUI_msleep(50);
	INT_1();      //INT=1;
#endif

	GUI_msleep(50);
	RST_1();      //RST=1;

	GUI_msleep(100); //Ҫ�ȴ�100mS!!!

	////����INTΪ����(������)
	cfg.direction =kGPIO_DigitalInput;
	GPIO_PinInit(CTP_INT_GPIO,CTP_INT_PIN,&cfg);
}

/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/


//GT9157 ????????????
//GT9157 ���ּĴ�����??
#define GT_CTRL_REG 	0X8040   	//GT9157���ƼĴ�??
#define GT_CFGS_REG 	0X8047   	//GT9157������檵�ַ�Ĵ�??
#define GT_CHECK_REG 	0X80FF   	//GT9157У��ͼĴ���?
#define GT_PID_REG 		0X8140   	//GT9157��ƷID�Ĵ�??

#define GT_GSTID_REG 	0X814E   	//GT9157��ǰ????���Ĵ������?
#define GT_TP1_REG 		0X8150  	//��һ����������ݵ��?
#define GT_TP2_REG 		0X8158		//�ڶ�����������ݵ��?
#define GT_TP3_REG 		0X8160		//�������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ���������ݵ��?
#define GT_TP5_REG 		0X8170		//�������������ݵ�ַ


// 5����GT9157������
static const u8 CTP_CFG_GT9157[] = {
	0x00,0x20,0x03,0xE0,0x01,0x05,0x3C,0x00,0x01,0x08,
	0x28,0x0C,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x17,0x19,0x1E,0x14,0x8B,0x2B,0x0D,
	0x33,0x35,0x0C,0x08,0x00,0x00,0x00,0x9A,0x03,0x11,
	0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
	0x00,0x20,0x58,0x94,0xC5,0x02,0x00,0x00,0x00,0x04,
	0xB0,0x23,0x00,0x93,0x2B,0x00,0x7B,0x35,0x00,0x69,
	0x41,0x00,0x5B,0x4F,0x00,0x5B,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
	0x12,0x14,0x16,0x18,0x1A,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
	0x10,0x12,0x13,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,
	0x21,0x22,0x24,0x26,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x48,0x01
};

// 7锟斤拷锟斤拷GT911锟斤拷锟斤拷锟斤�?
static const u8 CTP_CFG_GT911[] = {
  0x00,0x20,0x03,0xE0,0x01,0x05,0x0D,0x00,0x01,0x08,
  0x28,0x0F,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8A,0x2A,0x0C,
  0x45,0x47,0x0C,0x08,0x00,0x00,0x00,0x02,0x02,0x2D,
  0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
  0x00,0x28,0x64,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
  0x9C,0x2C,0x00,0x8F,0x34,0x00,0x84,0x3F,0x00,0x7C,
  0x4C,0x00,0x77,0x5B,0x00,0x77,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
  0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
  0x22,0x24,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
  0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x24,0x01	
};


// 4.3锟斤拷锟斤拷GT5688锟斤拷锟斤拷锟斤�?
const u8 CTP_CFG_GT5688[] =  {
0x97,0xE0,0x01,0x10,0x01,0x05,0x0D,0x00,0x01,0x00,
0x00,0x05,0x50,0x3C,0x53,0x11,0x00,0x00,0x11,0x11,
0x14,0x14,0x14,0x22,0x0A,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x53,0x00,0x14,0x00,0x00,0x84,0x00,0x00,
0x3C,0x00,0x00,0x64,0x1E,0x28,0x87,0x27,0x08,0x32,
0x34,0x05,0x0D,0x20,0x33,0x60,0x13,0x02,0x24,0x00,
0x00,0x64,0x80,0x80,0x14,0x02,0x00,0x00,0x54,0xC5,
0x26,0xA5,0x2E,0x90,0x36,0x80,0x3F,0x75,0x47,0x6C,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
0xFF,0xFF,0x07,0x00,0x00,0x00,0x02,0x14,0x14,0x03,
0x04,0x00,0x21,0x64,0x0A,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x32,0x20,0x50,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,
0x0D,0x06,0x0C,0x05,0x0B,0x04,0x0A,0x03,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x3C,0x00,0x05,0x1E,0x00,0x02,
0x2A,0x1E,0x19,0x14,0x02,0x00,0x03,0x0A,0x05,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0x86,
0x22,0x03,0x00,0x00,0x33,0x00,0x0F,0x00,0x00,0x00,
0x50,0x3C,0x50,0x00,0x00,0x00,0x2F,0xE1,0x01
};


static u8 config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

static TOUCH_IC touchIC;


static BOOL GTP_I2C_Read(u8 *buf, int len)
{
	int i;
		pIIC->Open(GT9xx_IIC_SPEED);

	    pIIC->Start();
	    if(!pIIC->WriteByte(GT_WR_ADDR,IIC_ACK))
	    {
	    	pIIC->Stop();

	    	pIIC->Close();
	    	return FALSE;
	    }
	    pIIC->WriteByte(buf[0],IIC_ACK);
		pIIC->WriteByte(buf[1],IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(GT_RD_ADDR,IIC_ACK);
		for (i = 0; i < len - 1; i++)
		{
		    buf[2+i] = pIIC->ReadByte(IIC_ACK);
		}

		// ????????���?
		buf[2+i] =pIIC->ReadByte(IIC_NoACK);

		pIIC->Stop();

		pIIC->Close();
		return TRUE;

}

static BOOL GTP_I2C_Write(u8 *buf,int len)
{
	int i;

	pIIC->Open(GT9xx_IIC_SPEED);

	pIIC->Start();
    if(!pIIC->WriteByte(GT_WR_ADDR,IIC_ACK))
    {
    	pIIC->Stop();

    	pIIC->Close();
    	return FALSE;
    }
    pIIC->WriteByte(buf[0],IIC_ACK);
	pIIC->WriteByte(buf[1],IIC_ACK);

	for (i = 0; i < len; i++)
	{
		pIIC->WriteByte(buf[2+i],IIC_ACK);
	}

	pIIC->Stop();

	pIIC->Close();
	return TRUE;
}

/**
  * @brief   使锟斤拷IIC锟斤拷取锟劫达拷锟斤拷荩锟斤拷锟斤拷锟斤拷欠锟斤拷锟�?
  * @param
  *		@arg client:锟斤拷锟斤拷�?
  *		@arg  addr: 锟侥达拷锟斤拷锟斤拷??
  *		@arg rxbuf: 锟芥储锟斤拷锟斤拷锟斤拷锟斤�??
  *		@arg len:    锟斤拷取锟斤拷锟街斤拷锟斤拷
  * @retval
  * 	@arg FAIL
  * 	@arg SUCCESS
  */
BOOL GTP_I2C_Read_dbl_check(u8 client_addr, uint16_t addr, u8 *rxbuf, int len)
{
    u8 buf[16] = {0};
    u8 confirm_buf[16] = {0};
    u8 retry = 0;
    
    GTP_DEBUG_FUNC();

    while (retry++ < 3)
    {
        memset(buf, 0xAA, 16);
        buf[0] = (u8)(addr >> 8);
        buf[1] = (u8)(addr & 0xFF);
        GTP_I2C_Read( buf, len + 2);
        
        memset(confirm_buf, 0xAB, 16);
        confirm_buf[0] = (u8)(addr >> 8);
        confirm_buf[1] = (u8)(addr & 0xFF);
        GTP_I2C_Read( confirm_buf, len + 2);

      
        if (!memcmp(buf, confirm_buf, len+2))
        {
            memcpy(rxbuf, confirm_buf+2, len);
            return TRUE;
        }
    }    
    GTP_ERROR("I2C read 0x%04X, %d bytes, double check failed!", addr, len);
    return FALSE;
}


/**
  * @brief   锟截憋拷GT91xx锟叫讹拷
  * @param ??
  * @retval ??
  */
void GTP_IRQ_Disable(void)
{

    GTP_DEBUG_FUNC();

    I2C_GTP_IRQDisable();
}

/**
  * @brief   使锟斤拷GT91xx锟叫讹拷
  * @param ??
  * @retval ??
  */
void GTP_IRQ_Enable(void)
{
    GTP_DEBUG_FUNC();
     
	  I2C_GTP_IRQEnable();    
}




//锟斤拷GT9157写锟斤拷????锟斤拷锟�?//reg:锟斤拷�d寄达拷锟斤拷锟斤�??
//buf:锟斤拷莼锟斤拷锟斤拷锟斤拷锟�?//len:写锟斤拷莩锟�?

static BOOL GTxx_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	int i;

	pIIC->Open(GT9xx_IIC_SPEED);

	pIIC->Start();
    if(!pIIC->WriteByte(GT_WR_ADDR,IIC_ACK))
    {
    	pIIC->Stop();

    	pIIC->Close();
    	return FALSE;
    }
    pIIC->WriteByte(reg>>8,IIC_ACK);
	pIIC->WriteByte(reg,IIC_ACK);

	for (i = 0; i < len; i++)
	{
		pIIC->WriteByte(buf[i],IIC_ACK);
	}

	pIIC->Stop();

	pIIC->Close();
	return TRUE;
}

//锟斤拷GT9157锟斤拷锟斤拷????锟斤拷锟�?//reg:锟斤拷�d寄达拷锟斤拷锟斤�??
//buf:锟斤拷莼锟斤拷锟斤拷锟斤拷锟�?//len:锟斤拷锟斤拷莩锟�?
static BOOL GTxx_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	int i;

	pIIC->Open(GT9xx_IIC_SPEED);

    pIIC->Start();
    if(!pIIC->WriteByte(GT_WR_ADDR,IIC_ACK))
    {
    	pIIC->Stop();

    	pIIC->Close();
    	return FALSE;
    }
    pIIC->WriteByte(reg>>8,IIC_ACK);
	pIIC->WriteByte(reg,IIC_ACK);

	pIIC->Start();
	pIIC->WriteByte(GT_RD_ADDR,IIC_ACK);
	for (i = 0; i < len - 1; i++)
	{
	    buf[i] = pIIC->ReadByte(IIC_ACK);
	}

	// ????????���?
	buf[i] =pIIC->ReadByte(IIC_NoACK);

	pIIC->Stop();

	pIIC->Close();
	return TRUE;
}

static const u16 GT9157_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};

static u8 tp_dev_sta=0;
static u16 tp_dev_x[8];
static u16 tp_dev_y[8];

static int	GTxx_Execu( int *x,int *y)
{
	u8 buf[4],mode,i,n;

	GTxx_RD_Reg(GT_GSTID_REG,&mode,1);//锟斤拷取锟斤拷锟斤拷锟斤拷锟阶�?
	if(mode&0x80)
	{
		n =mode&0xF;
		if((n>0) && (n<6))
		{
			//������?
			for(i=0;i<n;i++)
			{
				GTxx_RD_Reg(GT9157_TPX_TBL[i],buf,4);	//锟斤拷取XY
				tp_dev_x[i]=((u16)buf[1]<<8)+buf[0];
				tp_dev_y[i]=((u16)buf[3]<<8)+buf[2];
			}

			*x =tp_dev_x[0];
			*y =tp_dev_y[0];

			mode =0;
			GTxx_WR_Reg(GT_GSTID_REG,&mode,1);	//锟斤拷锟�?
			return 1;
		}

	}
	mode =0;
	GTxx_WR_Reg(GT_GSTID_REG,&mode,1);	//锟斤拷锟�?
	return	 0;
}




 /**
   * @brief   锟斤拷锟斤拷睡锟斤拷模式
   * @param ??
   * @retval 1为锟缴癸拷锟斤拷锟斤拷锟斤拷为失??
   */
//s8 GTP_Enter_Sleep(void)
//{
//    s8 ret = -1;
//    s8 retry = 0;
//    u8 reset_comment[3] = {(u8)(GTP_REG_COMMENT >> 8), (u8)GTP_REG_COMMENT&0xFF, 5};//5
//
//    GTP_DEBUG_FUNC();
//
//    while(retry++ < 5)
//    {
//        ret = GTP_I2C_Write(GTP_ADDRESS, reset_comment, 3);
//        if (ret > 0)
//        {
//            GTP_INFO("GTP enter sleep!");
//
//            return ret;
//        }
//
//    }
//    GTP_ERROR("GTP send sleep cmd failed.");
//    return ret;
//}

#if 0
s8 GTP_Send_Command(u8 command)
{
    s8 ret = -1;
    s8 retry = 0;
    u8 command_buf[3] = {(u8)(GTP_REG_COMMAND >> 8), (u8)GTP_REG_COMMAND&0xFF, GTP_COMMAND_READSTATUS};

    GTP_DEBUG_FUNC();

    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, command_buf, 3);
        if (ret > 0)
        {
            GTP_INFO("send command success!");
            return ret;
        }

    }
    GTP_ERROR("send command fail!");
    return ret;
}
#endif

#if 0
/**
  * @brief   锟斤拷锟窖达拷锟斤拷??
  * @param ??
  * @retval 0为锟缴癸拷锟斤拷锟斤拷锟斤拷为失??
  */
s8 GTP_WakeUp_Sleep(void)
{
    u8 retry = 0;
    s8 ret = -1;

    GTP_DEBUG_FUNC();

    while(retry++ < 10)
    {
        ret = GTP_I2C_Test();
        if (ret > 0)
        {
            GTP_INFO("GTP wakeup sleep.");
            return ret;
        }
        GTP_Reset_Guitar();
    }

    GTP_ERROR("GTP wakeup sleep failed.");
    return ret;
}
#endif

#if 0
static int GTP_Get_Info(void)
{
    u8 opr_buf[6] = {0};
    int ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    u8 int_trigger_type = GTP_INT_TRIGGER;
        
    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 6);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
    
    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    
    GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max,int_trigger_type);
    
    return SUCCESS;    
}

#endif



int GTP_Read_Version(void)
{
    int32_t ret = -1;
    uint8_t buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};    //锟侥达拷锟斤拷锟斤拷�?

    touchIC = UNKNOW;
    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read( buf, sizeof(buf));
    if (ret < 0)
    {
        GTP_ERROR("GTP read version failed");
        return ret;
    }

    if (buf[1] == '@')
    {
    	if(x_strncmp(&buf[2],"911",3)==0)
    	{
    		touchIC = GT911;
    	}
    	else if(x_strncmp(&buf[2],"9157",4)==0)
    	{
    		touchIC = GT9157;
    	}
    	else if(x_strncmp(&buf[2],"5688",4)==0)
    	{
    		touchIC = GT5688;
    	}

    }
    return ret;
}


BOOL GT9xx_Init(void)
{
    int ret = -1;
    int i = 0;
    int retry = 0;
    const u8* cfg_info;
    u8 check_sum = 0;
    u8 cfg_info_len  ;
    u8 cfg_num = 0x80FE - 0x8047+1 ;		//????锟斤拷锟矫的寄达拷锟斤拷锟斤拷锟斤拷

    pIIC->Init();
    GTP_PortInit();

	//锟斤拷取锟斤拷锟斤拷IC锟斤拷锟斤拷??
    GTP_Read_Version();
		
	//锟斤拷锟�?锟斤拷锟酵猴拷指锟斤拷同锟斤拷锟斤拷锟斤�?
    switch(touchIC)
    {
    case GT9157:
		cfg_info =  CTP_CFG_GT9157;
		cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT9157);//�������ñ�Ĵ��?
		break;
		////
    case GT911:
		cfg_info =  CTP_CFG_GT911;
		cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT911);//�������ñ�Ĵ��?
		break;
		////
    case GT5688:
		cfg_info =  CTP_CFG_GT5688;
		cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT5688);//�������ñ�Ĵ��?
		break;
		////
    default:
    	return FALSE;
    }


    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info, cfg_info_len);
		
    //锟斤拷锟斤拷要写锟斤拷zhecksum锟侥达拷锟斤拷锟斤拷??
    check_sum = 0;
    for (i = GTP_ADDR_LENGTH; i < cfg_num+GTP_ADDR_LENGTH; i++)
    {
        check_sum += config[i];
    }
    config[ cfg_num+GTP_ADDR_LENGTH] = (~check_sum) + 1; 	//checksum
    config[ cfg_num+GTP_ADDR_LENGTH+1] =  1; 						//refresh 锟斤拷锟矫革拷锟铰憋拷志

    //写锟斤拷锟斤拷锟斤拷锟斤拷息
    for (retry = 0; retry < 5; retry++)
    {

        ret = GTP_I2C_Write( config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
        GUI_msleep(50);
    }

    GUI_msleep(100);

		
	//使锟斤拷锟叫断ｏ拷锟斤拷锟斤拷锟斤拷軛锟斤拷獯ワ拷锟斤拷锟�
	//I2C_GTP_IRQEnable();
    //GTP_Get_Info();
    return TRUE;
}



/*============================================================================*/

static int ts_x=0,ts_y=0;
static int ts_down=0;

static int GetState(void)
{

#if 0
	u8 mode;

	GTxx_RD_Reg(GT_GSTID_REG,&mode,1);//锟斤拷取锟斤拷锟斤拷锟斤拷锟阶�?
	mode &= 0xF;
	return mode;
#endif

	int x,y;
	if(GTxx_Execu(&x,&y))
	{
		ts_x =x;
		ts_y =y;
		ts_down =1;
		return TRUE;
	}
	ts_down=0;
	return FALSE;


}

static BOOL GetSample(POINT *pt)
{
	if(ts_down==1)
	{
		pt->x =ts_x;
		pt->y =ts_y;
		return TRUE;
	}
	return FALSE;
}

static BOOL GetPoint(POINT *pt)
{
	if(GetSample(pt))
	{
#if	GUI_TOUCHSCREEN_CALIBRATE
		TouchPanel_TranslatePoint(LCD_XSIZE,LCD_YSIZE,pt);
#endif
		return TRUE;
	}
	return	TRUE;
}



static BOOL LoadCfg(TS_CFG_DATA *cfg)
{

	return TRUE;
}

static BOOL SaveCfg(TS_CFG_DATA *cfg)
{
	return TRUE;
}

/*============================================================================*/

const GUI_TOUCH_DEV TouchDev_GT9xx=
{
	.Init 		=GT9xx_Init,
	.GetState 	=GetState,
	.GetSample	=GetSample,
	.GetPoint	=GetPoint,
	.LoadCfg	=LoadCfg,
	.SaveCfg	=SaveCfg,
};

/*============================================================================*/


