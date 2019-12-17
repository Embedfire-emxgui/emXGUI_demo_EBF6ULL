

#include 	"BSP.h"
#include	"GUI_Drv_Cfg.h"
#include	"GUI_Drv.h"

/*============================================================================*/

#define	LCD_ClockInit_30MHZ()	elcdif_clock_init(30,1,4,6) //LCD_CLK =24*30/1/4/6=30MHZ
#define	LCD_ClockInit_36MHZ()	elcdif_clock_init(30,1,4,5) //LCD_CLK =24*30/1/4/5=36MHZ
#define	LCD_ClockInit_45MHZ()	elcdif_clock_init(30,1,4,4) //LCD_CLK =24*30/1/4/4=45MHZ
#define	LCD_ClockInit_60MHZ()	elcdif_clock_init(30,1,4,3) //LCD_CLK =24*30/1/4/3=60MHZ
#define	LCD_ClockInit_80MHZ()	elcdif_clock_init(30,1,3,3) //LCD_CLK =24*30/1/3/3=80MHZ

/*============================================================================*/

#if 1
//野火5.0 / 7.0TFT,800x480
#define	LCD_ClockInit()	elcdif_clock_init(30,1,6,5) //LCD_CLK =24*30/1/6/5=24MHZ

#define	LCD_ClockInit	LCD_ClockInit_30MHZ

#define HSW 1   //1
#define HFP 22  //22
#define HBP 46  //46
#define VSW 1   //1
#define VFP 22  //22
#define VBP 23  //23
#define POL_FLAGS \
    (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnRisingClkEdge)

#endif

#if 0
//iTOP-iMX6UL 7.0TFT,800x1280(塑料外壳)
//#define	LCD_ClockInit()	elcdif_clock_init(30,1,4,4) //LCD_CLK =24*30/1/4/4=45MHZ
//#define	LCD_ClockInit()	elcdif_clock_init(30,1,4,3) //LCD_CLK =24*30/1/4/3=60MHZ
//#define	LCD_ClockInit()	elcdif_clock_init(30,1,3,3) //LCD_CLK =24*30/1/3/3=80MHZ
#define	LCD_ClockInit	LCD_ClockInit_60MHZ

#define VBP    20
#define VFP    20
#define VSW    50

#define HBP    20
#define HFP    20
#define HSW    3

#define POL_FLAGS (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnFallingClkEdge)

#endif
////

#if 0
//iTOP-iMX6UL 7.0TFT,800x1280(金属外壳)
#define	LCD_ClockInit()	elcdif_clock_init(30,1,4,4) //LCD_CLK =24*30/1/4/4=45MHZ

#define VBP    50
#define VFP    50
#define VSW    3

#define HBP    50
#define HFP    50
#define HSW    3

#define POL_FLAGS (kELCDIF_DataEnableActiveLow | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnFallingClkEdge)

#endif
////

//LCD Backlight
#define	LCD_BL_IOMUXC_INIT()	IOMUXC_MUX_GPIO1->IO08.U =5; IOMUXC_CFG_GPIO1->IO08.U =0x10B0;
#define	LCD_BL_GPIO	GPIO1
#define	LCD_BL_PIN	8

//LCD RESET or DISP
#if 0
#define	LCD_DISP_IOMUXC_INIT()	IOMUXC_MUX_SNVS->TAMPER9.U =5; IOMUXC_CFG_SNVS->TAMPER9.U =0x10B0;
#define	LCD_DISP_GPIO	GPIO5
#define	LCD_DISP_PIN	9
#endif
////
void lcdif_display_setup(u32 fb_addr);

void LCD_HardInit(u32 fb_addr)
{
	//LCD控制器使用的是物理地址.


	LCD_ClockInit();
	elcdif_port_init(TRUE);
	elcdif_init(LCD_XSIZE,LCD_YSIZE,fb_addr,HSW,HFP,HBP,VSW,VFP,VBP,POL_FLAGS);

	gpio_pin_config_t cfg;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	////背光 - 0:OFF;1:ON
#ifdef	LCD_BL_GPIO
	LCD_BL_IOMUXC_INIT();
	GPIO_PinInit(LCD_BL_GPIO,LCD_BL_PIN,&cfg);
	GPIO_PinWrite(LCD_BL_GPIO,LCD_BL_PIN,1);
#endif

	////LCD_DISP- 0:OFF;1:ON
#ifdef	LCD_DISP_GPIO
	LCD_DISP_IOMUXC_INIT();
	GPIO_PinInit(LCD_DISP_GPIO,LCD_DISP_PIN,&cfg);

	GPIO_PinWrite(LCD_DISP_GPIO,LCD_DISP_PIN,0);
	SYS_msleep(50);
	GPIO_PinWrite(LCD_DISP_GPIO,LCD_DISP_PIN,1);
	SYS_msleep(50);
#endif

}



/*===================================================================================*/

void LCD_BkLight(int onoff)
{
	if(onoff)
	{
		GPIO_PinWrite(LCD_BL_GPIO,LCD_BL_PIN,1);
	}
	else
	{
		GPIO_PinWrite(LCD_BL_GPIO,LCD_BL_PIN,0);
	}
}

/*===================================================================================*/

