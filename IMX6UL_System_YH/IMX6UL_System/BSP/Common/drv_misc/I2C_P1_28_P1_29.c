
#include <stddef.h>

#include "Kernel.h"
#include "BSP.h"

/*=========================================================================================*/

static void SCL_IOMUXC_Init()
{
	IOMUXC_MUX_UART->TXD4.U =5;
	IOMUXC_CFG_UART->TXD4.U =0x10B0;
}
#define	SCL_GPIO	GPIO1
#define	SCL_PIN		28

static void	SDA_IOMUXC_Init()
{
	IOMUXC_MUX_UART->RXD4.U =5;
	IOMUXC_CFG_UART->RXD4.U =0x10B0;
}
#define	SDA_GPIO	GPIO1
#define	SDA_PIN		29

//IO操作函数
#if 1
#define IIC_SCL(x)		if(x){SCL_GPIO->DR |= (1<<SCL_PIN);}else{SCL_GPIO->DR &= ~(1<<SCL_PIN);}
#define IIC_SDA(x)		if(x){SDA_GPIO->DR |= (1<<SDA_PIN);}else{SDA_GPIO->DR &= ~(1<<SDA_PIN);}
#define READ_SDA		(SDA_GPIO->PSR&(1<<SDA_PIN))   		//读取SDA

#endif


#define	PIN_CONFIG	0x10B0u

#if 0
static const u32 PIN_CONFIG = SRE_1_FAST_SLEW_RATE
 |DSE_7_R0_7
 |SPEED_1_MEDIUM_100MHz
 |PKE_0_PULL_KEEPER_DISABLED
 |ODE_1_OPEN_DRAIN_ENABLED
 |PUE_1_PULL_SELECTED
 |PUS_3_22K_OHM_PULL_UP;
#endif

/*=========================================================================================*/

static void SDA_IN(void)
{
	SDA_GPIO->GDIR &= ~(1<<SDA_PIN);
}

static	void SDA_OUT(void)
{
	SDA_GPIO->GDIR |=  (1<<SDA_PIN);
}



//初始化IIC
static BOOL IIC_PortInit(void)
{


	SCL_IOMUXC_Init();
	SDA_IOMUXC_Init();

	if(1)
	{
		gpio_pin_config_t pin_cfg;

		pin_cfg.direction =kGPIO_DigitalOutput;
		pin_cfg.interruptMode =kGPIO_NoIntmode;
		pin_cfg.outputLogic =1;

		GPIO_PinInit(SCL_GPIO, SCL_PIN, &pin_cfg);
		GPIO_PinInit(SDA_GPIO, SDA_PIN, &pin_cfg);
	}



	return TRUE;
}

/*=========================================================================================*/

#define		IIC_MASTER_NAME I2C_P1_28_P1_29

#define		IIC_DELAY_COUNT	380 //for 100KHZ
#include 	"I2C_Master.c"

/*=========================================================================================*/

