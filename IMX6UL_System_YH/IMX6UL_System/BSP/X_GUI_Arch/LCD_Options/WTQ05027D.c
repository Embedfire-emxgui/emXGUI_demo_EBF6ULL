#include	"GUI_Drv.h"

/*============================================================================*/

void WTQ05027D_Init(int xsize,int ysize,int format,struct LCD_TIMING_CFG *cfg)
{  

	cfg->VBPD	=10;
	cfg->VFPD	=10;
	cfg->VSPW	=13;
	
	cfg->HBPD	=20;
	cfg->HFPD	=20;
	cfg->HSPW	=26;
	
	cfg->DPL	=1;//1
	cfg->EPL	=1;//1
	cfg->HSPL	=1;//1
	cfg->VSPL	=1;//1
	
	cfg->PCLK   =30;

  	GUI_Printf("WTQ05027D_Init:%d,%d\r\n",xsize,ysize);

  
} 

/*============================================================================*/
