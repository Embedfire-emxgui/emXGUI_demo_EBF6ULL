#include	"GUI_Drv.h"

/*============================================================================*/
#if 0
typedef struct	__vga_cfg{

	int xsize;
	int ysize;
	int FPS;

   	int HFPD;
   	int HSPW;
	int HBPD;
   	
 	int VFPD;
	int VSPW;
	int VBPD;

  	
   	int HSPL;
   	int VSPL;
   	
   	int PCLK;
   	
   	
}vga_cfg_t;
#endif

/*============================================================================*/
  
void VGA_800x600_Init(int xsize,int ysize,int fps,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:40MHZ
	
	cfg->HFPD = 40;
	cfg->HSPW = 128;
	cfg->HBPD = 88;
	
	cfg->VFPD = 1;
	cfg->VSPW = 4;
	cfg->VBPD = 23;
  
	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 1;
	cfg->VSPL = 1;
	
	cfg->PCLK = 40;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dfps\r\n",xsize,ysize,fps);
	
}

void VGA_1024x768_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:65MHZ

	cfg->HFPD = 24;
	cfg->HSPW = 136;
	cfg->HBPD = 160;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 29;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 1;
	cfg->VSPL = 1;

	cfg->PCLK = 65;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dfps\r\n",xsize,ysize,bpp);



}

void VGA_1280x800_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:83.5MHZ

	cfg->HFPD = 72;
	cfg->HSPW = 128;
	cfg->HBPD = 200;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 22;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 1;
	cfg->VSPL = 1;

	cfg->PCLK = 84;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);



}

void VGA_1366x768_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:85.5MHZ

	cfg->HFPD = 64;
	cfg->HSPW = 112;
	cfg->HBPD = 256;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 18;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 1;
	cfg->VSPL = 1;

	cfg->PCLK = 86;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);



}

void VGA_1440x900_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:88.75MHZ

	cfg->HFPD = 48;
	cfg->HSPW = 32;
	cfg->HBPD = 80;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 17;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 0;
	cfg->VSPL = 0;

	cfg->PCLK = 89;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);


}

void VGA_1600x1200_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:162MHZ

	cfg->HFPD = 64;
	cfg->HSPW = 192;
	cfg->HBPD = 304;

	cfg->VFPD = 1;
	cfg->VSPW = 3;
	cfg->VBPD = 46;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 0;
	cfg->VSPL = 0;

	cfg->PCLK = 162;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);


}

void VGA_1680x1050_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:119MHZ

#if 0 //CVT
	cfg->HFPD = 48;
	cfg->HSPW = 32;
	cfg->HBPD = 80;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 21;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 0;
	cfg->VSPL = 0;

	cfg->PCLK = 119;
#endif

#if 1
	cfg->HFPD = 104;
	cfg->HSPW = 176;
	cfg->HBPD = 280;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 30;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 0;
	cfg->VSPL = 0;

	cfg->PCLK = 146;
#endif


    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);


}

void VGA_1920x1080_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	// fps:60Hz,pclk:154MHZ

	cfg->HFPD = 48;
	cfg->HSPW = 32;
	cfg->HBPD = 80;

	cfg->VFPD = 3;
	cfg->VSPW = 6;
	cfg->VBPD = 26;

	cfg->DPL  = 0;
	cfg->EPL  = 1;

	cfg->HSPL = 0;
	cfg->VSPL = 0;

	cfg->PCLK = 154;

    GUI_Printf("VGA_Dev_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);


}

/*============================================================================*/



/*============================================================================*/
