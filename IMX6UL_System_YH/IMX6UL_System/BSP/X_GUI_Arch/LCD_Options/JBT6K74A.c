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

void	JBT6K74A_Init(int xsize,int ysize,int bpp,struct LCD_TIMING_CFG *cfg)
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

	delay_ms(10);
	
	//// Deep Standby Out
	write_cmd(0x00);
	delay_ms(10);
	write_cmd(0x00);
	delay_ms(10);
	write_cmd(0x00);
	delay_ms(10);
	
	write_cmd(0xB0);	//Deep Standby Out;
	write_dat(0x17);	
	
	//// Sleep Out
	write_cmd(0xBC);	//RGB I/F ON 
	write_dat(0x80);
	
	write_cmd(0x3B);	//Quad mode OFF
	write_dat(0x00);
	
	write_cmd(0xB0);	//AVDD ON, XVDD ON
	write_dat(0x16);
	
	write_cmd(0xB8);	//Output control
	write_dat(0xFF);
	write_dat(0xF9);
	
	write_cmd(0x11);	//Sleep mode OFF
	
	//// Initial Setting
	write_cmd(0xBA);	//Display mode setup(1)
	write_dat(0x01);
	
	write_cmd(0xBB);	//Disaplay mode setup(2)
	write_dat(0x00);
	
	write_cmd(0x3A);	//RGB Interface data format
	write_dat(0x60);
	
	write_cmd(0xBF);	//Drive system change control
	write_dat(0x10);
	
	write_cmd(0xB1);	//Booster operation setup
	write_dat(0x56);
	
	write_cmd(0xB2);	//Booster mode setup
	write_dat(0x33);
	
	write_cmd(0xB3);	//Booster frequencies setup
	write_dat(0x11);
	
	write_cmd(0xB4);	//Operational amplifier capability / System colode...
	write_dat(0x02);
	
	write_cmd(0xB5);	//VCS voltage adjustment
	write_dat(0x35);
	
	write_cmd(0xB6);	//VCOM voltage adjustment
	write_dat(0x40);
	
	write_cmd(0xB7);	//Configure an external display signal
	write_dat(0x03);
	
	write_cmd(0xB9);	//DCCLK and DCEV timing setup
	write_dat(0x04);
	
	write_cmd(0xBD);	//ASW signal slew rate adjustment
	write_dat(0x00);
	
	write_cmd(0xBE);	//Dummy display count setup for quad data operation
	write_dat(0x00);
	
	write_cmd(0xC0);	//Sleep-out FR count setup(A)
	write_dat(0x11);
	
	write_cmd(0xC1);	//Sleep-out FR count setup(B)
	write_dat(0x11);
	
	write_cmd(0xC2);	//Sleep-out FR count setup(C)
	write_dat(0x11);
	
	write_cmd(0xC3);	//Sleep-in line clock count setup(D)
	write_dat(0x20);
	write_dat(0x40);
	
	write_cmd(0xC4);	//Sleep-in line clock count setup(E)
	write_dat(0x60);
	write_dat(0xC0);
	
	write_cmd(0xC5);	//Sleep-in line clock count setup(F)
	write_dat(0x10);
	write_dat(0x20);
	
	write_cmd(0xC6);	//Sleep-in line clock count setup(G)
	write_dat(0x60);
	write_dat(0xC0);
	
	write_cmd(0xC7);	//Gamma 1 fine tuning(1)
	write_dat(0x33);
	write_dat(0x43);
	
	write_cmd(0xC8);	//Gamma 1 fine tuning(2)
	write_dat(0x44);
	
	write_cmd(0xC9);	//Gamma 1 inclination adjustment
	write_dat(0x33);
	
	write_cmd(0xCA);	//Gamma 1 blue offset adjustment
	write_dat(0x00);
	
	write_cmd(0xEC);	//Total number of horizontal clock cycles(1)
	write_dat(0x02);
	write_dat(0x68);
	
	write_cmd(0xCF);	//Blanking period control (1)
	write_dat(0x02);

	write_cmd(0xD0);	//Blanking period control (2)
	write_dat(0x08);
	write_dat(0x04);
	
	write_cmd(0xD1);	//CKV timing control on/off
	write_dat(0x01);
	
	write_cmd(0xD2);	//CKV1, 2 timing control
	write_dat(0x14);
	write_dat(0x00);
	
	write_cmd(0xD3);	//OEV timing control
	write_dat(0x1A);
	write_dat(0x0F);
	
	write_cmd(0xD4);	//ASW timing control (1)
	write_dat(0x1F);
	write_dat(0xAF);
	
	write_cmd(0xD5);	//ASW timing control (2)
	write_dat(0x14);
	
	////
	write_cmd(0x29);	//Display ON
	
  	GUI_Printf("JBT6K74A_Init:%d,%d,%dBPP\r\n",xsize,ysize,bpp);
	
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

