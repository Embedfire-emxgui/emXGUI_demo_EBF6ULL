
#include	"GUI_Drv.h"

/*============================================================================*/


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


/*============================================================================*/
/*============================================================================*/


void	R61581_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{
	cfg->VBPD		=10;
	cfg->VFPD		=10;
	cfg->VSPW		=10;
	
	cfg->HBPD		=10;
	cfg->HFPD		=10;
	cfg->HSPW		=10;
	
	cfg->DPL	=1;
	cfg->EPL	=1;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	
	LCD_SPI_Init();
	
	LCD_SPI_CS(1);
	LCD_SPI_SCLK(0);
	LCD_SPI_MOSI(0);

#define	W_COM	write_cmd
#define	W_DATA	write_dat
#define	Delayms	delay_ms

W_COM(0xFF);
W_COM(0xFF);
Delayms(5);
W_COM(0xFF);
W_COM(0xFF);
W_COM(0xFF);
W_COM(0xFF);
Delayms(10);

W_COM(0xB0);
W_DATA(0x00);

W_COM(0xB3);
W_DATA(0x02);
W_DATA(0x00);
W_DATA(0x00);
W_DATA(0x00);

W_COM(0xC0);
W_DATA(0x03);
W_DATA(0x3B);//480
W_DATA(0x00);
W_DATA(0x00);
W_DATA(0x00);
W_DATA(0x01);
W_DATA(0x00);//NW
W_DATA(0x43);

W_COM(0xC1);
W_DATA(0x08);
W_DATA(0x1C);//CLOCK
W_DATA(0x08);
W_DATA(0x08);

W_COM(0xC4);
W_DATA(0x11);
W_DATA(0x02);
W_DATA(0x13);
W_DATA(0x01);

W_COM(0xC6);
//W_DATA(0x00);
W_DATA((1<<4)|(1<<3)|(1<<1)|(1<<0)); 

W_COM(0xC8);//GAMMA
W_DATA(0x02);
W_DATA(0x04);
W_DATA(0x0E);
W_DATA(0xBA);
W_DATA(0x0D);
W_DATA(0x0D);
W_DATA(0x03);
W_DATA(0x06);
W_DATA(0x20);
W_DATA(0x10);

W_DATA(0x06);
W_DATA(0x03);
W_DATA(0x0D);
W_DATA(0xBD);
W_DATA(0x0A);
W_DATA(0x0E);
W_DATA(0x04);
W_DATA(0x02);
W_DATA(0x10);
W_DATA(0x20);

W_COM(0x35);
W_DATA(0x00);

  	W_COM(0x36); 
 
	if( xsize==480 && ysize==320)
	{
  		W_DATA((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
	}

	if( xsize==320 && ysize==480)
	{
  		W_DATA((0<<7)|(0<<6)|(0<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
	}
 Delayms(5);

W_COM(0x3A);
W_DATA(0x66);

W_COM(0x44);
W_DATA(0x00);
W_DATA(0x01);

W_COM(0xD0);
W_DATA(0x07);
W_DATA(0x07);//VCI1
W_DATA(0x18);//VRH
W_DATA(0x06);//BT

W_COM(0xD1);
W_DATA(0x03);
W_DATA(0x56);//VCM
W_DATA(0x10);//VDV

W_COM(0xD2);
W_DATA(0x03);
W_DATA(0x14);
W_DATA(0x04);

W_COM(0x11);
Delayms(150);

W_COM(0x29);
Delayms(30);

W_COM(0x2A);
W_DATA(0x00);
W_DATA(0x00);
W_DATA(xsize>>8);
W_DATA(xsize);//320

W_COM(0x2B);
W_DATA(0x00);
W_DATA(0x00);
W_DATA(ysize>>8);
W_DATA(ysize);//480

W_COM(0xB4);
W_DATA(0x10);
Delayms(100);

W_COM(0xB0);
W_DATA(0x03);

W_COM(0x2C);
  	
  	GUI_Printf("R61581_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);
 	
  	
}



/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

