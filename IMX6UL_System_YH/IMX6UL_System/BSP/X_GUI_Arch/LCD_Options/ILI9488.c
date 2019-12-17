#include	"GUI_Drv.h"

/*============================================================================*/


/*============================================================================*/


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

static	void  delay(void)
{
  volatile	int i;
  for(i=0;i<20;i++);
}


/*============================================================================*/

static	void write_cmd (unsigned char y) // (uchar y,uchar x)
{
  	unsigned char i;

	LCD_SPI_CS(0);
	delay();
	LCD_SPI_SCLK(0);
	delay();	
	LCD_SPI_MOSI(0);
	delay();	
	LCD_SPI_SCLK(1);
	delay();

	for(i=0;i<8;i++)
    {
     	LCD_SPI_SCLK(0);
     	delay();
      	if (y&0x80)
      	{
          LCD_SPI_MOSI(1);
      	}
      	else
      	{
          LCD_SPI_MOSI(0);
      	}
       	LCD_SPI_SCLK(1);
       	delay();
      	y=y<<1;
    }
	LCD_SPI_CS(1);
}
/*
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
*/
/////////////////////////////////////////////////
static	void write_dat(unsigned char w) // (uchar w, uchar v)
{
  	unsigned char i;
  	
	LCD_SPI_CS(0);
	delay();
	LCD_SPI_SCLK(0);
	delay();		
	LCD_SPI_MOSI(1);
	delay();
	LCD_SPI_SCLK(1);
	delay();

	for(i=0;i<8;i++)
    {
     	LCD_SPI_SCLK(0);
     	delay();
      	if (w&0x80)
      	{
          LCD_SPI_MOSI(1);
      	}
      	else
      	{
          LCD_SPI_MOSI(0);
      	}
      	LCD_SPI_SCLK(1);
      	delay();
      	w=w<<1;
    }
	LCD_SPI_CS(1);
	
}

/*
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
*/

 
/*============================================================================*/
void ILI9488_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
{  

	cfg->VBPD		=5;
	cfg->VFPD		=5;
	cfg->VSPW		=1;
	
	cfg->HBPD		=5;
	cfg->HFPD		=5;
	cfg->HSPW		=1;
	
	cfg->DPL	=0;
	cfg->EPL	=0;
	cfg->HSPL	=0;
	cfg->VSPL	=0;
	
	LCD_SPI_Init();
	
	LCD_SPI_CS(1);
	LCD_SPI_SCLK(0);
	LCD_SPI_MOSI(0);

  /***************************************/ 
write_cmd(0xE0); 
write_dat(0x00); 
write_dat(0x07); 
write_dat(0x0f); 
write_dat(0x0D); 
write_dat(0x1B); 
write_dat(0x0A); 
write_dat(0x3c); 
write_dat(0x78); 
write_dat(0x4A); 
write_dat(0x07); 
write_dat(0x0E); 
write_dat(0x09); 
write_dat(0x1B); 
write_dat(0x1e); 
write_dat(0x0f);  
 
write_cmd(0xE1); 
write_dat(0x00); 
write_dat(0x22); 
write_dat(0x24); 
write_dat(0x06); 
write_dat(0x12); 
write_dat(0x07); 
write_dat(0x36); 
write_dat(0x47); 
write_dat(0x47); 
write_dat(0x06); 
write_dat(0x0a); 
write_dat(0x07); 
write_dat(0x30); 
write_dat(0x37); 
write_dat(0x0f); 

write_cmd(0xC0); 
write_dat(0x10); 
write_dat(0x10); 
 
write_cmd(0xC1); 
write_dat(0x41); 

write_cmd(0xC5); 
write_dat(0x00); 
write_dat(0x22); 
write_dat(0x80); 

write_cmd(0x36); 
if( xsize==480 && ysize==320)
{
  	write_dat((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
}

if( xsize==320 && ysize==480)
{
  	write_dat((0<<7)|(1<<6)|(0<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
}

write_cmd(0x3A); //Interface Mode Control
write_dat(0x66);

write_cmd(0xB0);  //Interface Mode Control  
write_dat(0x00); 

write_cmd(0xB1);   //Frame rate 70HZ  
write_dat(0xB0); 
write_dat(0x11); 

write_cmd(0xB4); 
write_dat(0x02); 

write_cmd(0xB5); 
write_dat(0x05); 
write_dat(0x05); 
write_dat(0x05); 
write_dat(0x05); 
 
write_cmd(0xB7); 
write_dat(0xC6); 

write_cmd(0xE9); 
write_dat(0x00);
 
write_cmd(0xF7);    
write_dat(0xA9); 
write_dat(0x51); 
write_dat(0x2C); 
write_dat(0x82);

/**********set rgb interface mode******************/
write_cmd(0xB6); 
write_dat(0x70); //30 set rgb
write_dat(0X02); //GS,SS 02£¬42£¬62
write_dat(0x3B);
 /**************************************************/ 
 
   	write_cmd(0x2A); 
  	write_dat(0x00); 
  	write_dat(0x00); 
  	write_dat(xsize>>8); 
  	write_dat(xsize); 

  	write_cmd(0x2B); 
  	write_dat(0x00); 
  	write_dat(0x00); 
  	write_dat(ysize>>8); 
  	write_dat(ysize); 
  	
/*
write_cmd(0x2A); //Frame rate control
write_dat(0x00);
write_dat(0x00);
write_dat(0x01);
write_dat(0x3F);

write_cmd(0x2B); //Display function control
write_dat(0x00);
write_dat(0x00);
write_dat(0x01);
write_dat(0xDF);
*/

write_cmd(0x11);
delay_ms(120);
write_cmd(0x29); //display on
write_cmd(0x2c);

 GUI_Printf("ILI9488_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 

/*
void oldILI9488_Init(int xsize,int ysize,int bpp,struct LCD_HW_CFG *cfg)
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

  	write_cmd(0x11); 
  	delay_ms(150);
  
	//Command Access Protect (B0h)
   	write_cmd(0xB0);
  	write_dat(0x00);

	write_cmd(0xB1);   //Frame rate 70HZ  
	write_dat(0xB0); 
	write_dat(0x11); 

  	//Display Mode and Frame Memory Write Mode Setting (B4h)
  	write_cmd(0xB4);
  	write_dat(0x02); 
  	
 	write_cmd(0xB7); 
	write_dat(0xC6); 

	write_cmd(0xE9); 
	write_dat(0x00);
 
	write_cmd(0XF7);    
	write_dat(0xA9); 
	write_dat(0x51); 
	write_dat(0x2C); 
	write_dat(0x82);
	 	
  	//Display Function Control (B6h)
  	write_cmd(0xB6); 
	write_dat(0x30); //30 set rgb
	write_dat(0x22); //GS,SS 02£¬42£¬62
	write_dat(0x3B);
    
    //Power_Setting (D0h)
   	write_cmd(0xD0);      
  	write_dat(0x07);    
  	write_dat(0x41);    
  	write_dat(0x1D);    

	//VCOM Control (D1h)
  	write_cmd(0xD1); 
  	write_dat(0x00);    
  	write_dat(0x0A);    
  	write_dat(0x08);    
    
    //Power_Setting for Normal Mode (D2h)
  	write_cmd(0xD2); 
  	write_dat(0x01);    
  	write_dat(0x11);    
    
    //Panel Driving Setting (C0h)
  	write_cmd(0xC0); 
  	write_dat(0x00); 
  	write_dat(0x3B); 
  	write_dat(0x00); 
  	write_dat(0x02); 
  	write_dat(0x11); 
    
    //Display_Timing_Setting for Normal Mode (C1h)
  	write_cmd(0xC1);
  	write_dat(0x10);
  	write_dat(0x18);
  	write_dat(0x88);
   	
	//Frame Rate and Inversion Control (C5h)
	write_cmd(0xC5);
	write_dat(0x00);

  	//Interface Control (C6h)
  	write_cmd(0xC6);       
  	write_dat((1<<4)|(1<<3)|(1<<1)|(1<<0)); 
  	
	write_cmd(0xC8);       
	write_dat(0x02);     
	write_dat(0x46);     
	write_dat(0x14);     
	write_dat(0x31);    
	write_dat(0x0a);     
	write_dat(0x04);     
	write_dat(0x37);     
	write_dat(0x24);     
	write_dat(0x57);     
	write_dat(0x13);     
	write_dat(0x06);     
	write_dat(0x0c);

  	write_cmd(0x36); 
	if( xsize==480 && ysize==320)
	{
  		write_dat((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
	}

	if( xsize==320 && ysize==480)
	{
  		write_dat((0<<7)|(1<<6)|(0<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
	}
	
   	//Vertical Scrolling Start Address (37h)
   	write_cmd(0x37);
   	write_dat(0x00);
   	write_dat(0x00);
   	
 	if(bpp==16)
 	{
  		write_cmd(0x3A); //65K 
  		write_dat(0x55); 
  	}
   	
 	if(bpp>=18)
 	{
  		write_cmd(0x3A); //262K 
  		write_dat(0x66); 
 	}

  	write_cmd(0x2A); 
  	write_dat(0x00); 
  	write_dat(0x00); 
  	write_dat(xsize>>8); 
  	write_dat(xsize); 

  	write_cmd(0x2B); 
  	write_dat(0x00); 
  	write_dat(0x00); 
  	write_dat(ysize>>8); 
  	write_dat(ysize); 
	
  	delay_ms(50); 
  	
   	//Enter_invert_mode (21h)
   	write_cmd(0x21);
 	delay_ms(10);
	
  	write_cmd(0x29); 
  	write_cmd(0x2C); 
  	GUI_Printf("ILI9488 Init:%dx%d,%dBPP\r\n",xsize,ysize,bpp);

  
} 
*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
