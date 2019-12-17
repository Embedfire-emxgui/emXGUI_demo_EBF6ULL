#include	"GUI_Drv.h"

/*============================================================================*/


/*============================================================================*/

static	void  delay(void)
{
  volatile	int i;
  for(i=0;i<1;i++);
}

static void delay_ms(int t)
{
	volatile int i;
	////
	
	while(t--)
	{
		for(i=0;i<50;i++);
	}
}


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
  
void RM68040_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg) 
{  

	
	cfg->VBPD	=10;
	cfg->VFPD	=10;
	cfg->VSPW	=10;
	
	cfg->HBPD	=10;
	cfg->HFPD	=10;
	cfg->HSPW	=10;
	
	cfg->DPL	=1;
	cfg->EPL	=1;
	cfg->HSPL	=1;
	cfg->VSPL	=1;
	
	LCD_SPI_Init();
	
	LCD_SPI_CS(1);
	LCD_SPI_SCLK(0);
	LCD_SPI_MOSI(0);

	delay_ms(10);
  	write_cmd(0x11); 
  	delay_ms(150);
  	 
  	write_cmd(0xB4);
  	write_dat(0x10); 
 
 	//// Power_Setting(D0h)
   	write_cmd(0xD0);      
  	write_dat(0x07);  
  	write_dat(0x41);   
  	write_dat(0x18);  
    
    //// VCOM Control(D1h)
  	write_cmd(0xD1); 
  	write_dat(0x00);    
  	write_dat(0x1F);    
  	write_dat(0x08);    
    
    //// Power_Setting for Normal Mode(D2h)
  	write_cmd(0xD2); 
  	write_dat(0x05);    
  	write_dat(0x22);    

  	    
    //// Panel Driving Setting (C0h)
  	write_cmd(0xC0); 
  	write_dat(0x00); 
  	write_dat(0x3B); 
  	write_dat(0x00); 
  	write_dat(0x02); 
  	write_dat(0x11); 
  
  	//// Display_Timing_Setting for Normal Mode (C1h)
  	write_cmd(0xC1);
  	write_dat(0x10);
  	write_dat(0x13);
  	write_dat(0x88);
  	write_dat(0xc5);
  	write_dat(0x02);
  		
	//// Frame Rate and Inversion Control (C5h)
	write_cmd(0xC5);
  	write_dat(0x02);
  	
  	//// Interface Control (C6h)
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
	if((xsize==480) && (ysize==320))
	{
  		write_dat((0<<7)|(0<<6)|(1<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
  	}

	if((xsize==320) && (ysize==480))
	{
  		write_dat((0<<7)|(1<<6)|(0<<5)|(1<<3)|(0<<2)|(0<<1)|(0<<0));
  	}
  	
   /*
 	if(bpp==16)
 	{
  		write_cmd(0x3A); //65K 
  		write_dat(0x55); 
  	}
   	
 	if(bpp>=18)*/
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
  	write_cmd(0x29); 
  	write_cmd(0x2C); 
    GUI_Printf("RM68040_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);
} 

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
