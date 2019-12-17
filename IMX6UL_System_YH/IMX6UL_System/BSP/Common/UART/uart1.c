

#include "bsp.h"
#include "uart_drv.h"

uint32_t GetUartSrcClock(void);


/*=======================================================================================*/

#define UART_BASE 			UART1
#define UART_CLK_FREQ 		GetUartSrcClock()
#define UART_IRQn 			UART1_IRQn
#define UART_IRQHandler 	UART1_IRQHandler
#define UART_PortInit 		UART1_PortInit


#define	RX_L1_BUF_SIZE 40
#define	RX_L2_BUF_SIZE 8*1024

static  u8 rxL1_buf[RX_L1_BUF_SIZE];
static	u8 rxL2_buf[RX_L2_BUF_SIZE];
static volatile s16 rxL2_idx=0;
static volatile s16 aa=0;

static	com_cfg_t uart_cfg;
static	sem_obj *sem_rx=NULL;


static void rx_buf_clean(void)
{
	rxL2_idx =0;
	memset(rxL2_buf,0,RX_L2_BUF_SIZE);
}

static int rx_buf_in(u8 *in,int size)
{
	int cnt;

	cnt =MIN(size,RX_L2_BUF_SIZE-rxL2_idx); //计算实际能写入的数量.
	if(cnt > 0)
	{
		memcpy(&rxL2_buf[rxL2_idx],in,cnt);
		rxL2_idx += cnt;
	}
	else
	{ //超出了...
		aa++;
	}
	return cnt;
}

static int rx_buf_out(u8 *out,int size)
{
	int cnt;

	cnt =MIN(size,rxL2_idx);	//计算实际能读出的数量.

	memcpy(out,rxL2_buf,cnt); //读出数据.
	memmove(rxL2_buf,&rxL2_buf[cnt],RX_L2_BUF_SIZE-cnt); //移动数据.
	rxL2_idx -= cnt;
	return cnt;
}

/*=======================================================================================*/

static volatile UART_Type *uart=UART_BASE;


/*=======================================================================================*/


static void uart_irq_enable(void)
{
	EnableIRQ(UART_IRQn);
}

static void uart_irq_disable(void)
{
	DisableIRQ(UART_IRQn);
}


static void rx_handler(void)
{
	volatile int n;

	n=0;
    while(uart->USR2 & UART_USR2_RDR_MASK)
    {
    	u16 data;

    	if(n >= RX_L1_BUF_SIZE)
    	{
    	   break;
    	}

       	data =uart->URXD;
    	rxL1_buf[n] =data&0xFF;
    	n++;
    }

    rx_buf_in(rxL1_buf,n);
    if(rxL2_idx > ((RX_L2_BUF_SIZE*2)>>2))
    {
    	SYS_sem_post(sem_rx);
    }
}


static void uart_isr(void)
{
    u32 sr1,sr2;

    volatile int a=0;

    sr1 =uart->USR1;
    sr2 =uart->USR2;

	if (UART_GetStatusFlag(UART_BASE, kUART_AgingTimerFlag))
	{
		SYS_sem_post(sem_rx);
	}

	if ((UART_GetStatusFlag(UART_BASE, kUART_RxDataReadyFlag)) || (UART_GetStatusFlag(UART_BASE, kUART_RxOverrunFlag)))
    {
    	rx_handler();
    }

	if (UART_GetStatusFlag(UART_BASE, kUART_RxDsFlag)) //RX IDLE
	{
	   SYS_sem_post(sem_rx);
	}

/*
    if(sr2&(1<<12)) //IDLE
    {
    	SYS_sem_post(sem_rx);
    	aa++;
    }
*/
	uart->USR1 =sr1;
	uart->USR2 =sr2;

}


/*=========================================================================================*/

#include "uart_drv.h"
static BOOL uart_set_cfg(com_cfg_t *new_cfg);

static BOOL uart_open(u32 baud)
{
	uart_config_t cfg;
	uart_transfer_t xfer;

	char str[] ="UART1 OK";

	sem_rx =SYS_sem_create(0,1,NULL);

	uart_cfg.Baudrate =baud;
	uart_cfg.RxTimeOut =200;
	uart_cfg.Flags	= COM_DATA_BITS_8
				| COM_STOP_BITS_1
				| COM_PARITY_NONE
				| COM_FLOW_CONTROL_NONE
				| COM_INTERFACE_NORMAL;

	UART_PortInit();
	uart_set_cfg(&uart_cfg);
	return TRUE;

}

static 	void uart_close(void)
{
	DisableIRQ(UART_IRQn);
}

static BOOL uart_set_cfg(com_cfg_t *new_cfg)
{
	uart_config_t cfg;

	memcpy(&uart_cfg,new_cfg,sizeof(com_cfg_t));

	UART_GetDefaultConfig(&cfg);

	cfg.baudRate_Bps  = uart_cfg.Baudrate;//921600;
	cfg.dataBitsCount = kUART_EightDataBits;
	cfg.stopBitCount  = kUART_OneStopBit;
	cfg.parityMode    = kUART_ParityDisabled;
	cfg.txFifoWatermark = 16;
	cfg.rxFifoWatermark = 16;


	cfg.enableTx = true;
	cfg.enableRx = true;

	DisableIRQ(UART_IRQn);

	UART_Init(UART_BASE, &cfg, UART_CLK_FREQ);
	UART_EnableInterrupts(UART_BASE, kUART_RxDataReadyEnable | kUART_RxOverrunEnable|kUART_AgingTimerEnable);
	SetIRQHandler(UART_IRQn, uart_isr);
	EnableIRQ(UART_IRQn);

	return TRUE;
}

static BOOL uart_get_cfg(com_cfg_t *cfg)
{
	memcpy(cfg,&uart_cfg,sizeof(com_cfg_t));
	return TRUE;
}

static int uart_write(const u8 *buf,int size)
{
	UART_WriteBlocking(UART_BASE, buf,size);
	return size;
}

static int uart_read(u8 *buf,int size)
{
	int i,cnt;

	i=0;
	while(size > 0)
	{
		uart_irq_disable();
		cnt =rx_buf_out(&buf[i],size);
		uart_irq_enable();
		if(cnt==0)
		{
			if(SYS_sem_wait(sem_rx,uart_cfg.RxTimeOut)!=SYS_NO_ERR)
			{
				break;
			}
		}
		else
		{
			i += cnt;
			size -= cnt;
		}
	}
	return i;
}

const struct uart_ops UART1_ops=
{
	uart_open,
	uart_close,
	uart_set_cfg,
	uart_get_cfg,
	uart_write,
	uart_read,
};

/*=========================================================================================*/
/*=========================================================================================*/

#if 1

static uint8_t g_tipString[] ="Uart1 Test:\r\n";

static volatile int delay=10;
static volatile int rd_n=10;
static	u8 rx_buf[8*1024];

int UART1_Test(void)
{

	delay=0;
	rd_n=20;

	uart_open(115200);
	uart_write(g_tipString, sizeof(g_tipString) / sizeof(g_tipString[0]));

    while (1)
    {
    	volatile int n;

    	n =uart_read(rx_buf,rd_n);

    	if(n>0)
    	{
    		uart_write(rx_buf,n);
    	}

    	if(delay>0)
    	{
    		SYS_msleep(delay);
    	}


    }
}

#endif


