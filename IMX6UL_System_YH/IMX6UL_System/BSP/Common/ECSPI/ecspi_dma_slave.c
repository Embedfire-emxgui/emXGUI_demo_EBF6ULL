#include "BSP.h"
#include "fsl_ecspi.h"
#include "fsl_ecspi_sdma.h"

/*=========================================================================================*/
/*=========================================================================================*/

#define	PRINTF(x,...)

#define EXAMPLE_ECSPI_SLAVE_BASEADDR ECSPI2
#define EXAMPLE_DMA SDMAARM
#define EXAMPLE_CHANNEL_RX 2
#define EXAMPLE_CHANNEL_TX 3
#define ECSPI_TX_DMA_REQUEST 10
#define ECSPI_RX_DMA_REQUEST 9
#define TRANSFER_SIZE 64U /*! Transfer dataSize */

/*=========================================================================================*/

static uint8_t slaveRxData[TRANSFER_SIZE] = {0U};

AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t masterRxData[TRANSFER_SIZE], 4) = {0U};
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t masterTxData[TRANSFER_SIZE], 4) = {0U};

AT_NONCACHEABLE_SECTION_ALIGN(static ecspi_sdma_handle_t g_ecspiSdmaHandle, 4) = {0};
AT_NONCACHEABLE_SECTION_ALIGN(static sdma_handle_t g_ecspiTxSdmaHandle, 4) = {0};
AT_NONCACHEABLE_SECTION_ALIGN(static sdma_handle_t g_ecspiRxSdmaHandle, 4) = {0};

AT_NONCACHEABLE_SECTION_ALIGN(static sdma_context_data_t contextTx, 4) = {0};
AT_NONCACHEABLE_SECTION_ALIGN(static sdma_context_data_t contextRx, 4) = {0};

static volatile bool isTransferCompleted = false;
static uint8_t dummy_buffer[TRANSFER_SIZE];

/*=========================================================================================*/

static void ECSPI_SlaveUserCallback(ECSPI_Type *base, ecspi_sdma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        PRINTF("This is ECSPI slave transfer completed callback. \r\n");
        PRINTF("It's a successful transfer. \r\n\r\n");
        isTransferCompleted = true;
    }

    if (status == kStatus_ECSPI_HardwareOverFlow)
    {
        PRINTF("Hardware overflow occured in this transfer. \r\n");
    }
}


/*=========================================================================================*/

void	ECSPI_DMA_SlaveTest(void)
{
	   CLOCK_SetMux(kCLOCK_EcspiMux, 0); /* Set ECSPI source to PLL3 60M */
	    CLOCK_SetDiv(kCLOCK_EcspiDiv, 0); /* Set ECSPI divider to 1 */

	    PRINTF("ECSPI board to board sdma example.\r\n");

	    uint32_t i;
	    ecspi_slave_config_t slaveConfig;
	    ecspi_transfer_t slaveXfer;
	    sdma_config_t sdmaConfig;
	    uint8_t dummy_count;

	    for (dummy_count = 0; dummy_count < TRANSFER_SIZE; dummy_count++)
	    {
	        dummy_buffer[dummy_count] = (uint8_t)ECSPI_DUMMYDATA;
	    }

	    /* Slave config:
	     * slaveConfig.channel = kECSPI_Channel0;
	     * slaveConfig.burstLength = 8;
	     * slaveConfig.txFifoThreshold = 1;
	     * slaveConfig.rxFifoThreshold = 0;
	     */
	    ECSPI_SlaveGetDefaultConfig(&slaveConfig);
	    ECSPI_SlaveInit(EXAMPLE_ECSPI_SLAVE_BASEADDR, &slaveConfig);

	    SDMA_GetDefaultConfig(&sdmaConfig);
	    SDMA_Init(EXAMPLE_DMA, &sdmaConfig);
	    SDMA_CreateHandle(&g_ecspiRxSdmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL_RX, &contextRx);
	    SDMA_CreateHandle(&g_ecspiTxSdmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL_TX, &contextTx);

	    /* Create ECSPI SDMA handle. */
	    ECSPI_SlaveTransferCreateHandleSDMA(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_ecspiSdmaHandle, ECSPI_SlaveUserCallback, NULL,
	                                        &g_ecspiTxSdmaHandle, &g_ecspiRxSdmaHandle, ECSPI_TX_DMA_REQUEST,
	                                        ECSPI_RX_DMA_REQUEST, EXAMPLE_CHANNEL_TX, EXAMPLE_CHANNEL_RX);

	    while (1)
	    {
	        PRINTF("\r\n Slave example is running...\r\n\r\n");

	        /* Reset the receive buffer */
	        for (i = 0; i < TRANSFER_SIZE; i++)
	        {
	            slaveRxData[i] = 0U;
	        }

	        /* Set slave transfer ready to receive data */
	        isTransferCompleted = false;
	        slaveXfer.txData = (uint32_t *)dummy_buffer;
	        ;
	        slaveXfer.rxData = (uint32_t *)slaveRxData;
	        slaveXfer.dataSize = TRANSFER_SIZE;
	        slaveXfer.channel = kECSPI_Channel0;

	        /* Slave start receive */
	        PRINTF("Slave starts to receive data!\r\n");

	        ECSPI_SlaveTransferSDMA(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_ecspiSdmaHandle, &slaveXfer);
	        SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL_RX, 3);
	        SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL_TX, 3);
	        while (!isTransferCompleted)
	        {
	        }

	        /* Set slave transfer ready to send back data */
	        isTransferCompleted = false;
	        slaveXfer.txData = (uint32_t *)slaveRxData;
	        slaveXfer.rxData = (uint32_t *)dummy_buffer;
	        slaveXfer.dataSize = TRANSFER_SIZE;
	        slaveXfer.channel = kECSPI_Channel0;

	        /* Slave start send */
	        PRINTF("Slave starts to transmit data!\r\n");
	        ECSPI_SlaveTransferSDMA(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_ecspiSdmaHandle, &slaveXfer);
	        SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL_RX, 3);
	        SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL_TX, 3);

	        while (!isTransferCompleted)
	        {
	        }

	        /* Print out receive buffer */
	        PRINTF("\r\n Slave receive:");
	        for (i = 0U; i < TRANSFER_SIZE; i++)
	        {
	            /* Print 16 numbers in a line */
	            if ((i & 0x0FU) == 0U)
	            {
	                PRINTF("\r\n    ");
	            }
	            PRINTF(" %02X", slaveRxData[i]);
	        }
	        PRINTF("\r\n");
	    }
}

/*=========================================================================================*/
/*=========================================================================================*/
