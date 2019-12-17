#include	"Include.h"
#include	"BSP.h"

/*=========================================================================================*/
/*=========================================================================================*/


#define	GPIO_PIN_STCP	GPIO5,7
#define	GPIO_PIN_OE		GPIO5,8
#define	GPIO_PIN_SDI	GPIO5,10
#define	GPIO_PIN_SHCP	GPIO5,11

#define	STCP(x) GPIO_PinWrite(GPIO_PIN_STCP,x)
#define	OE(x) 	GPIO_PinWrite(GPIO_PIN_OE,x)
#define	SDI(x)	GPIO_PinWrite(GPIO_PIN_SDI,x)
#define	SHCP(x)	GPIO_PinWrite(GPIO_PIN_SHCP,x)

static volatile u8 HC595_DR=0;

static mutex_obj *lock=NULL;

void	HC595_WritePort(u8 val);
void	HC595_SetPin(u8 pin,u8 val);

void PortInit_HC595(void)
{
	gpio_pin_config_t cfg;


	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

#if 0
	////±³¹â.
	rIOMUXC_MUX(MUX_GPIO1_IO08).U =0x05;
	rIOMUXC_CFG(CFG_GPIO1_IO08).U =0x10B0;
	GPIO_PinInit(GPIO1,8,&cfg);
	GPIO_PinWrite(GPIO1,8,1);
#endif

	//// STCP
	IOMUXC_MUX_SNVS->TAMPER7.U =0x05;
	IOMUXC_CFG_SNVS->TAMPER7.U =0x10B0;
	GPIO_PinInit(GPIO_PIN_STCP,&cfg);

	//// nOE
	IOMUXC_MUX_SNVS->TAMPER8.U =0x05;
	IOMUXC_CFG_SNVS->TAMPER8.U =0x10B0;
	GPIO_PinInit(GPIO_PIN_OE,&cfg);

	//// SDI
	rIOMUXC_MUX(MUX_BOOT_MODE0).U =0x05;
	rIOMUXC_CFG(CFG_BOOT_MODE0).U =0x10B0;
	GPIO_PinInit(GPIO_PIN_SDI,&cfg);

	//// SHCP
	rIOMUXC_MUX(MUX_BOOT_MODE1).U =0x05;
	rIOMUXC_CFG(CFG_BOOT_MODE1).U =0x10B0;
	GPIO_PinInit(GPIO_PIN_SHCP,&cfg);

	lock =SYS_mutex_create(NULL);

	SDI(0);
	SHCP(0);
	STCP(0);
	OE(0);

	HC595_DR =0;

	HC595_SetPin(7,1); //LCD_PWREN - 0:OFF;1:ON.

}

static void delay(volatile int n)
{
	volatile int i;

	while(n-- > 0)
	{
		for(i=0;i<10;i++);
	}
}

static void write(u8 dat)
{
	char i;

	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		{
			SDI(1);
		}
		else
		{
			SDI(0);
		}
		dat <<= 1;

		delay(3);
		SHCP(0);
		delay(3);
		SHCP(1);
	}

	delay(3);
	STCP(0);
	delay(3);
	STCP(1);
	delay(3);

//	OE(0);
}

void	HC595_WritePort(u8 val)
{
	SYS_mutex_lock(lock,1000);
	HC595_DR = val;
	write(HC595_DR);
	SYS_mutex_unlock(lock);

}

void	HC595_SetPin(u8 pin,u8 val)
{

	SYS_mutex_lock(lock,1000);
	if(val==0)
	{
		HC595_DR &= ~(1<<pin);
	}
	else
	{
		HC595_DR |= (1<<pin);
	}
	write(HC595_DR);
	SYS_mutex_unlock(lock);

}
/*=========================================================================================*/
