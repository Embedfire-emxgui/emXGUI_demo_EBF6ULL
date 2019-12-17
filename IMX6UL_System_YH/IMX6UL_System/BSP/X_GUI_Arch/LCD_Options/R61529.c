 
#include	"GUI_Drv.h"

/*============================================================================*/


static	void  delay(void)
{
  volatile	int i;
  for(i=0;i<2;i++);
}

static void delay_ms(int t)
{
	volatile int i;
	////
	
	while(t--)
	{
		for(i=0;i<200;i++);
	}
}



/*============================================================================*/


/*============================================================================*/


/*============================================================================*/


static	void	write_cmd(U8 cmd)
{
	LCD_SPI_CS(0);
	delay();
	
	LCD_SPI_SCLK(0);
	delay();	

	LCD_SPI_MOSI(0);
	delay();
	
	LCD_SPI_SCLK(1);
	delay();
	
	LCD_SPI_RW(cmd);
	delay();
	
	LCD_SPI_CS(1);
	delay_ms(2);
}

/////////////////////////////////////////////////

static	void	write_dat(U8 dat)
{

	LCD_SPI_CS(0);
	delay();
	
	LCD_SPI_SCLK(0);
	delay();		
	
	LCD_SPI_MOSI(1);
	delay();
	
	LCD_SPI_SCLK(1);
	delay();
	
	LCD_SPI_RW(dat);
	delay();

	LCD_SPI_CS(1);
	delay_ms(2);
}
 
void R61529_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
 
	cfg->VBPD		=10;
	cfg->VFPD		=10;
	cfg->VSPW		=10;
	
	cfg->HBPD		=10;
	cfg->HFPD		=10;
	cfg->HSPW		=10;
	
	cfg->DPL	=0;
	cfg->EPL	=0;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	
	LCD_SPI_Init();
	
	LCD_SPI_CS(1);
	LCD_SPI_SCLK(0);
	LCD_SPI_MOSI(0);
	
 	
#if 1

#define	    LCDSPI_InitCMD	 write_cmd
#define	    LCDSPI_InitDAT	 write_dat
#define		WaitTime		delay_ms

LCDSPI_InitCMD(0x10);
WaitTime(100);
LCDSPI_InitCMD(0x11);
WaitTime(100);

LCDSPI_InitCMD(0xB0);
LCDSPI_InitDAT(0x04);
WaitTime(20);

if(xsize==320 && ysize==480)
{
	LCDSPI_InitCMD(0x36);
	LCDSPI_InitDAT(0x00);	
}

if(xsize==480 && ysize==320)
{
	LCDSPI_InitCMD(0x36);
	LCDSPI_InitDAT((1<<5));	
}
WaitTime(20);


LCDSPI_InitCMD(0x3A);//FIXEL FORMATE(DPI,MDDI.DSI)
LCDSPI_InitDAT(0x66);//5-16 6-18 7-24

LCDSPI_InitCMD(0xB4);//Display Mode and Frame Memory Write Mode Setting
LCDSPI_InitDAT(0x00);//00

LCDSPI_InitCMD(0x0A);
LCDSPI_InitDAT(0x0C);// get_power mode


LCDSPI_InitCMD(0xC0);//Panel Driving Setting
LCDSPI_InitDAT(0x03);//02 //REV, SM, GS, BGR, SS
LCDSPI_InitDAT(0xDF);//DF //NL[7:0], 63h:400Lines Type A, E3h:400Lines Type B
LCDSPI_InitDAT(0x40);//NL[8], SCN[6:0]
LCDSPI_InitDAT(0x10);//BLV, PTV => DOT / Column Inv Selection ///3:Column 0:dot
LCDSPI_InitDAT(0x10);//NDL PTDC
LCDSPI_InitDAT(0x01);//PTG,ISC[3:0]
LCDSPI_InitDAT(0x00);//BLS
LCDSPI_InitDAT(0x43);//PCDIVH[3:0], PCDIVL[3:0]

LCDSPI_InitCMD(0xC1);//Display Timing Setting for Normal Mode
LCDSPI_InitDAT(0x07);//DIV[3:0]
LCDSPI_InitDAT(0x28); //RTN[5:0]
LCDSPI_InitDAT(0x04);//BP[7:0]
LCDSPI_InitDAT(0x04);//FP[7:0]
LCDSPI_InitDAT(0x10);//5=Column

LCDSPI_InitCMD(0xC4);//Source/Gate Driving Timing Setting
LCDSPI_InitDAT(0x27); //NOWB[2:0], NOW[2:0] 20
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x03);//SEQGND[3:0] 03
LCDSPI_InitDAT(0x01); //SEQVCIL[3:0]

LCDSPI_InitCMD(0xC8);//Gamma
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);//26
LCDSPI_InitDAT(0x30);//32
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);
LCDSPI_InitDAT(0x30);
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);

LCDSPI_InitCMD(0xC9);//Gamma
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);//26
LCDSPI_InitDAT(0x30);//32
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);
LCDSPI_InitDAT(0x30);
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);

LCDSPI_InitCMD(0xCA);//Gamma
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);//26
LCDSPI_InitDAT(0x30);//32
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x16);
LCDSPI_InitDAT(0x24);
LCDSPI_InitDAT(0x30);
LCDSPI_InitDAT(0x48);
LCDSPI_InitDAT(0x3d);
LCDSPI_InitDAT(0x28);
LCDSPI_InitDAT(0x20);
LCDSPI_InitDAT(0x14);
LCDSPI_InitDAT(0x0c);
LCDSPI_InitDAT(0x04);

LCDSPI_InitCMD(0xD0);//Power Setting(charge Pump Setting)
LCDSPI_InitDAT(0x95);//A6//DC4(8+VCL) ----- Short time
LCDSPI_InitDAT(0x0E);//06 max, 0e min //BT
LCDSPI_InitDAT(0x08);
LCDSPI_InitDAT(0x20);//10 //DC1M(VSN) //long 10 short 20
LCDSPI_InitDAT(0x31);//1E //VC1[2:0]2.82V VC2[2:0]4V
LCDSPI_InitDAT(0x04);
LCDSPI_InitDAT(0x01);
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x08);
LCDSPI_InitDAT(0x01);
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x06);
LCDSPI_InitDAT(0x01);
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x20);

LCDSPI_InitCMD(0xD1);//VCOM Setting
LCDSPI_InitDAT(0x02);// WCVDC
LCDSPI_InitDAT(0x10);// PVH (PVLVL) [6:0] //2A/23
LCDSPI_InitDAT(0x00);// NVH (NVLVL) [6:0] //10
LCDSPI_InitDAT(0x2C);//40 ¡Ú VCOM VDC[7:0] best set<42>-->3a(flicker) -->2c (re Flicker)//2C

LCDSPI_InitCMD(0x2a);// set cloumn adrdress
LCDSPI_InitDAT(0);
LCDSPI_InitDAT(0);
LCDSPI_InitDAT((xsize-1)>>8);
LCDSPI_InitDAT((xsize-1));

LCDSPI_InitCMD(0x2B);// set page adrdress
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT(0x00);
LCDSPI_InitDAT((ysize-1)>>8);
LCDSPI_InitDAT((ysize-1));

LCDSPI_InitCMD(0xB0);
LCDSPI_InitDAT(0x03);

LCDSPI_InitCMD(0x11);
WaitTime(240);
LCDSPI_InitCMD(0x29);
  
GUI_Printf("R61529_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

#endif

 }
 