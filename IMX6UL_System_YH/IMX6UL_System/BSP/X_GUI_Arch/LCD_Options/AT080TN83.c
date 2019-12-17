#include	"GUI_Drv.h"

/*============================================================================*/

void AT080TN83_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  

	cfg->VBPD		=23;
	cfg->VFPD		=10;
	cfg->VSPW		=10;
	
	cfg->HBPD		=46;
	cfg->HFPD		=210;
	cfg->HSPW		=20;
	
	cfg->DPL	=1;
	cfg->EPL	=1;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	

  	GUI_Printf("AT080TN83_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 

/*============================================================================*/
