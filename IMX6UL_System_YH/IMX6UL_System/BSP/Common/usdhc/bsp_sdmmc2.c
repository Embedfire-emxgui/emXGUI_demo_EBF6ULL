

#include <stdio.h>

#include "bsp.h"
#include "bsp_sdcard.h"
#include "fsl_common.h"

#if 0

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define	PRINTF(x,...)


#define USDHC2_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U))


/*! @brief Data block count accessed in card */
#define DATA_BLOCK_COUNT (5U)
/*! @brief Start data block number accessed in card */
#define DATA_BLOCK_START (2U)
/*! @brief Data buffer size */
#define DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * DATA_BLOCK_COUNT)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
* @brief printf the card information log.
*
* @param card Card descriptor.
*/
static void CardInformationLog(mmc_card_t *card);
void USDHC2_DriverIRQHandler(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

mmc_card_t g_mmc;
static mmc_card_t *card = &g_mmc;


////////////////

uint8_t *g_sdmmc=NULL;


//////////////////
static sys_hook_obj tick_hook;

static void sd_on_tick(void)
{
	usdhc_ontick(5);
}

static void sd2_isr(void)
{
	mk_isr_enter();
	USDHC2_DriverIRQHandler();
	mk_isr_exit();
}

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint8_t *g_dataWrite =NULL;
static uint8_t *g_dataRead =NULL;

/*!
 * @brief Main function
 */
int sd2_test(void)
{
#if 0
    bool isReadOnly;
    bool failedFlag = false;
    char ch = '0';

    USDHC2_PortInit();

//	mk_tick_hook_create(&tick_hook,sd_on_tick);
	register_interrupt_routine(IMX_INT_USDHC2, sd2_isr);

 //   SystemInitIrqTable();

    CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0); // 0:PLL2_PFD2; 1:PLL2_PFD0.
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 0);

 //   g_sdmmc =dma_mem_alloc(SDMMC_GLOBAL_BUFFER_SIZE);

    /* Save host information. */
    card->host.base = USDHC2;
    card->host.sourceClock_Hz = USDHC2_CLK_FREQ;

    /* Init card. */
    if (MMC_Init(card))
    {
        PRINTF("\r\nSD card init failed.\r\n");
        return -1;
    }
    /* card information log */
    CardInformationLog(card);
#endif

    g_dataRead =dma_mem_alloc(512*16);

    int sec,count;
    sec=0;
    count=1;
    while(1)
    {
    	int sta;

		sta =MMC_ReadBlocks(card, g_dataRead, sec, count);
		DCACHE_InvalidateByRange(g_dataRead,count*512);
		sec += count;
		SYS_msleep(100);
    }

    SD_Deinit(card);

    while (true)
    {
    	SYS_msleep(100);
    	SYS_msleep(100);
    }
}

static void CardInformationLog(mmc_card_t *card)
{
    assert(card);

    PRINTF("\r\nCard user partition size %d * %d bytes\r\n", card->blockSize, card->userPartitionBlocks);
    PRINTF("\r\nWorking condition:\r\n");

    if (card->hostVoltageWindowVCC == kMMC_VoltageWindows270to360)
    {
        PRINTF("\r\n  Voltage: VCC - 2.7V ~ 3.3V");
    }
    else if (card->hostVoltageWindowVCC == kMMC_VoltageWindow170to195)
    {
        PRINTF("\r\n  Voltage: VCC - 1.7V ~ 1.95V");
    }
    if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindows270to360)
    {
        PRINTF("  VCCQ - 2.7V ~ 3.3V\r\n");
    }
    else if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow170to195)
    {
        PRINTF("  VCCQ - 1.7V ~ 1.95V\r\n");
    }
    else if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow120)
    {
        PRINTF("  VCCQ - 1.2V\r\n");
    }

    if (card->currentTiming == kMMC_HighSpeedTimingNone)
    {
        PRINTF("\r\n  Timing mode: Default");
    }
    else if ((card->currentTiming == kMMC_HighSpeed26MHZTiming) || (card->currentTiming == kMMC_HighSpeed52MHZTiming))
    {
        PRINTF("\r\n  Timing mode: High Speed\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeed200Timing)
    {
        PRINTF("\r\n  Timing mode: HS200\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeed400Timing)
    {
        PRINTF("\r\n  Timing mode: HS400\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeedDDR52Timing)
    {
        PRINTF("\r\n  Timing mode: DDR52\r\n");
    }

    if (card->currentTiming == kMMC_HighSpeedDDR52Timing)
    {
        PRINTF("\r\n  Freq : %d HZ\r\n", card->busClock_Hz / 2U);
    }
    else
    {
        PRINTF("\r\n  Freq : %d HZ\r\n", card->busClock_Hz);
    }
}

/*=========================================================================================*/
#define	SD_DMA_COUNT 8
static u8 *sd_dma_buf=NULL;

BOOL SDMMC2_DrvInit(void)
{
	mmc_card_t *card = &g_mmc;
	bool isReadOnly;
	bool failedFlag = false;

	memset(&g_mmc,0,sizeof(mmc_card_t));

	USDHC2_PortInit();

	mk_tick_hook_create(&tick_hook,sd_on_tick);
	register_interrupt_routine(IMX_INT_USDHC2, sd2_isr);


	CLOCK_SetDiv(kCLOCK_Usdhc2Div, 0);
	CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0); // 0:PLL2_PFD2; 1:PLL2_PFD0.

	g_sdmmc =dma_mem_alloc(SDMMC_GLOBAL_BUFFER_SIZE);

	sd_dma_buf =dma_mem_alloc(SD_DMA_COUNT*512);

	card->isHostReady =FALSE;
	card->host.base = USDHC2;
	card->host.sourceClock_Hz = USDHC2_CLK_FREQ;
	card->hostVoltageWindowVCCQ = kMMC_VoltageWindows270to360;
	card->hostVoltageWindowVCC = kMMC_VoltageWindows270to360;
	if (MMC_Init(card)!=kStatus_Success)
	{
		PRINTF("\r\nSD card init failed.\r\n");
		return FALSE;
	}

	CardInformationLog(card);

	sd2_test();
	return TRUE;
}

static void	__memcpy(u8 *dst,const u8 *src,int count)
{

	switch(count & 0x7)
	{
		case 7: count &= ~0x07; goto out7;
		case 6: count &= ~0x07; goto out6;
		case 5: count &= ~0x07; goto out5;
		case 4: count &= ~0x07; goto out4;
		case 3: count &= ~0x07; goto out3;
		case 2: count &= ~0x07; goto out2;
		case 1: count &= ~0x07; goto out1;
		default: break;
	}

	while(count > 0)
	{
		count -= 8;

		out8: *dst++ =*src++;
		out7: *dst++ =*src++;
		out6: *dst++ =*src++;
		out5: *dst++ =*src++;

		out4: *dst++ =*src++;
		out3: *dst++ =*src++;
		out2: *dst++ =*src++;
		out1: *dst++ =*src++;
	}
}


BOOL SDMMC2_drvReadBlock(u8 *buf,u32 blk_addr,int blk_count)
{
	status_t s;
	int i,n;

	for(i=0;i<blk_count;i+=n)
	{
		n =MIN(SD_DMA_COUNT,blk_count-i);
		s=MMC_ReadBlocks(card,sd_dma_buf,blk_addr+i,n);
		if(s!=kStatus_Success)
		{
			if (MMC_Init(card)!=kStatus_Success)
			{
				return FALSE;
			}

		}
		else
		{
			u8 *dst;
			L1C_InvalidateDataCacheRange((uint32_t*)sd_dma_buf,(n<<9));
			//SCB_InvalidateDCache_by_Addr((uint32_t*)sd_dma_buf,(n<<9));
			dst =buf;
			dst +=i*512;
			__memcpy(dst,sd_dma_buf,n<<9);
		}
	}
	return TRUE;

}

BOOL SDMMC2_drvWriteBlock(u8 *buf,u32 blk_addr,int blk_count)
{
	status_t s;
	int i,n;


	for(i=0;i<blk_count;i+=n)
	{
		n =MIN(SD_DMA_COUNT,blk_count-i);
		__memcpy(sd_dma_buf,&buf[i<<9],n<<9);

		s=MMC_WriteBlocks(card,sd_dma_buf,blk_addr+i,n);
		if(s!=kStatus_Success)
		{
			if (MMC_Init(card)!=kStatus_Success)
			{
				return FALSE;
			}

		}

	}
	return TRUE;

}
/*=========================================================================================*/
/*=========================================================================================*/
/*=========================================================================================*/
/*=========================================================================================*/
#endif
