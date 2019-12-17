#include	"GUI_Drv.h"

/*============================================================================*/

void AT070TN83_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  

	cfg->VBPD		=23;
	cfg->VFPD		=23;
	cfg->VSPW		=23;
	
	cfg->HBPD		=46;
	cfg->HFPD		=46;
	cfg->HSPW		=46;
	
	cfg->DPL	=1;
	cfg->EPL	=1;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	

  	GUI_Printf("AT070TN83_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 

/*============================================================================*/
