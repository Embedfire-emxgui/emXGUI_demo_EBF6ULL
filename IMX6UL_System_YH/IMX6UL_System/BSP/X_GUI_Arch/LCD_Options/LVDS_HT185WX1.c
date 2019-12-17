#include	"GUI_Drv.h"

/*============================================================================*/

void HT185WX1_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  //LVDS,1366x768(1344x768)

	cfg->VBPD		=8;
	cfg->VFPD		=10;
	cfg->VSPW		=5;
	
	cfg->HBPD		=10;
	cfg->HFPD		=50;
	cfg->HSPW		=5;
	
	cfg->DPL	=1;
	cfg->EPL	=1;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	

  	GUI_Printf("HT185WX1_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 

/*============================================================================*/
