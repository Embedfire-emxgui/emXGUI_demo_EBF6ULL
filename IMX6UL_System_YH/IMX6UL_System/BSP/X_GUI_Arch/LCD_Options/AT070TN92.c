#include	"GUI_Drv.h"

/*============================================================================*/

void AT070TN92_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  

	cfg->VBPD		=10;
	cfg->VFPD		=10;
	cfg->VSPW		=13;
	
	cfg->HBPD		=20;
	cfg->HFPD		=20;
	cfg->HSPW		=26;
	
	cfg->DPL	=1;//1
	cfg->EPL	=1;//1
	cfg->HSPL	=0;//0
	cfg->VSPL	=0;//0
	

  
} 

/*============================================================================*/
