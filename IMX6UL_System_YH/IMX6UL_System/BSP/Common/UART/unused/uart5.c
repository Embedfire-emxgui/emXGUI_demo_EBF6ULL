

#include "bsp.h"
#include "uart_drv.h"

uint32_t GetUartSrcClock(void);


/*=======================================================================================*/

#define UART_BASE 			LPUART5
#define UART_CLK_FREQ 		GetUartSrcClock()
#define UART_IRQn 			LPUART5_IRQn
#define UART_IRQHandler 	LPUART5_IRQHandler
#define UART_PortInit 		UART5_Pin_Init


#define	RX_L1_BUF_SIZE 128
#define	RX_L2_BUF_SIZE 1024

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

static volatile LPUART_Type *uart=UART_BASE;


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
	int n;

	n=0;
    while(1)
    {
    	u16 data;

    	if(n >= RX_L1_BUF_SIZE)
    	{
    	   aa++;
    	}

       	data =uart->DATA;
    	if(data&(1<<12))
    	{
    		break;
    	}

    	rxL1_buf[n] =data&0xFF;
    	n++;
    }

    rx_buf_in(rxL1_buf,n);
    if(rxL2_idx > ((RX_L2_BUF_SIZE*2)>>2))
    {
    	SYS_sem_post(sem_rx);
    }
}


void UART_IRQHandler(void)
{
    u32 sta;

    mk_isr_enter();

    sta =LPUART_GetStatusFlags(UART_BASE);

    if(sta&kLPUART_IdleLineFlag)
    {
    	SYS_sem_post(sem_rx);
    	aa++;
    }

    if(sta&kLPUART_RxFifoUnderflowFlag)
    {
    	aa++;
    }

    if(sta&kLPUART_RxDataRegFullFlag)
    {
    	rx_handler();
    }
    LPUART_ClearStatusFlags(UART_BASE,sta);

    mk_isr_exit();
}


/*=========================================================================================*/

#include "uart_drv.h"

static BOOL uart_open(u32 baud)
{
	lpuart_config_t cfg;


	sem_rx =SYS_sem_create(0,1,NULL);

	uart_cfg.Baudrate =baud;
	uart_cfg.RxTimeOut =200;
	uart_cfg.Flags	= COM_DATA_BITS_8
				| COM_STOP_BITS_1
				| COM_PARITY_NONE
				| COM_FLOW_CONTROL_NONE
				| COM_INTERFACE_NORMAL;

	UART_PortInit();

	LPUART_GetDefaultConfig(&cfg);
	cfg.baudRate_Bps  = baud;//921600;
	cfg.dataBitsCount = kLPUART_EightDataBits;
	cfg.stopBitCount  = kLPUART_OneStopBit;
	cfg.parityMode    = kLPUART_ParityDisabled;
	cfg.rxIdleType    = kLPUART_IdleTypeStopBit;
	cfg.rxIdleConfig  = kLPUART_IdleCharacter128;
	cfg.txFifoWatermark = 0;
	cfg.rxFifoWatermark = 0;

	cfg.enableTx = true;
	cfg.enableRx = true;

	LPUART_Init(UART_BASE, &cfg, UART_CLK_FREQ);

	/* Enable RX interrupt. */
	LPUART_EnableInterrupts(UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
	LPUART_EnableInterrupts(UART_BASE, kLPUART_IdleLineInterruptEnable);
	EnableIRQ(UART_IRQn);
	return TRUE;

}

static 	void uart_close(void)
{
	DisableIRQ(UART_IRQn);
	LPUART_DisableInterrupts(UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
	LPUART_DisableInterrupts(UART_BASE, kLPUART_IdleLineInterruptEnable);
}

static BOOL uart_set_cfg(com_cfg_t *new_cfg)
{
	lpuart_config_t cfg;

	memcpy(&uart_cfg,new_cfg,sizeof(com_cfg_t));


	LPUART_GetDefaultConfig(&cfg);

	cfg.baudRate_Bps  = uart_cfg.Baudrate;//921600;
	cfg.dataBitsCount = kLPUART_EightDataBits;
	cfg.stopBitCount  = kLPUART_OneStopBit;
	cfg.parityMode    = kLPUART_ParityDisabled;
	cfg.rxIdleType    = kLPUART_IdleTypeStopBit;
	cfg.rxIdleConfig  = kLPUART_IdleCharacter128;
	cfg.txFifoWatermark = 0;
	cfg.rxFifoWatermark = 0;

	cfg.enableTx = true;
	cfg.enableRx = true;

	LPUART_Init(UART_BASE, &cfg, UART_CLK_FREQ);

	/* Enable RX interrupt. */
	LPUART_EnableInterrupts(UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
	LPUART_EnableInterrupts(UART_BASE, kLPUART_IdleLineInterruptEnable);
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
	LPUART_WriteBlocking(UART_BASE, buf,size);
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

const struct uart_ops UART5_ops=
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

