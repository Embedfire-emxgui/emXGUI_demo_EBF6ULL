
#include	"BSP.h"

/*============================================================================================*/

//static const struct	 uart_ops *pUART= &UART0_ops;

/*============================================================================================*/

/*=========================================================================================*/

void	dbg_puts(const char *s)
{
	//pUART->write(s,x_strlen(s));
}


/*=========================================================================================*/
static	int q1=0x1234;
static	int q3;
int q4;

/*=========================================================================================*/

int	DebugUart_Init(void)
{
	static int q2=0x5678;
	int	i;
	char buf[128];
	U32 *p;
	
	//pUART->Open(115200);

	dbg_puts("\r\n");
	dbg_puts("********************************************\r\n");	
	x_sprintf(buf,"* Build Date: %s %s\r\n",__DATE__,__TIME__);dbg_puts(buf);
	x_sprintf(buf,"* TEXT: %08XH - %08XH; Size: %d\r\n",__text_base,__text_end,__text_end-__text_base);dbg_puts(buf);
	x_sprintf(buf,"* DATA: %08XH - %08XH; Size: %d\r\n",__data_base,__data_end,__data_end-__data_base);dbg_puts(buf);
	x_sprintf(buf,"* BSS:  %08XH - %08XH; Size: %d\r\n",__bss_base,__bss_end,__bss_end-__bss_base);dbg_puts(buf);
	x_sprintf(buf,"* [%08X][%08X][%08X][%08X]\r\n",q1,q2,q3,q4);dbg_puts(buf);
	dbg_puts("********************************************\r\n");	

#if 0
	x_sprintf(buf,"UPLL:   %dMHz.\r\n",UPLL_Clk);dbg_puts(buf);
	x_sprintf(buf,"APLL:   %dMHz.\r\n",APLL_Clk);dbg_puts(buf);
	x_sprintf(buf,"SYSCLK: %dMHz.\r\n",SYS_Clk);dbg_puts(buf);
	x_sprintf(buf,"PCLK:   %dMHz.\r\n",PCLK);dbg_puts(buf);
	x_sprintf(buf,"CPUCLK: %dMHz.\r\n",CPU_Clk);dbg_puts(buf);

	x_sprintf(buf,"HCLK1:   %dMHz.\r\n",sysGetClock(SYS_HCLK1));dbg_puts(buf);
	x_sprintf(buf,"HCLK234: %dMHz.\r\n",sysGetClock(SYS_HCLK234));dbg_puts(buf);

#endif

	//x_sprintf(buf,"Fclk=%dMHZ,Hclk=%dMHZ,Pclk=%d\r\n",CPU_GetFclkHZ()/1000000,CPU_GetHclkHZ()/1000000,CPU_GetPclkHZ()/1000000);
   // dbg_puts(buf);

	return	TRUE;

}

/*=========================================================================================*/
/*=========================================================================================*/

/*============================================================================================*/


int	SysDrv_stdin(char *buf,int size)
{
 	return	-1;
}

int	SysDrv_stdout(const char *buf,int size)
{
	return	0;//pUART->Write(buf,size);

}

int	SysDrv_stderr(const char *buf,int size)
{
	return	0;//pUART->Write(buf,size);
}

/*============================================================================================*/
