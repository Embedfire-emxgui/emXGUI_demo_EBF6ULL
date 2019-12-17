#include	"GUI_Drv.h"

/*============================================================================*/

void AT050TN22_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  
	cfg->VBPD		=6;
	cfg->VFPD		=6;
	cfg->VSPW		=6;
	
	cfg->HBPD		=20;
	cfg->HFPD		=20;
	cfg->HSPW		=123;
	
	cfg->DPL	=0;
	cfg->EPL	=0;
	cfg->HSPL	=0;
	cfg->VSPL	=0;

  	GUI_Printf("AT050TN22_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 

/*============================================================================*/
