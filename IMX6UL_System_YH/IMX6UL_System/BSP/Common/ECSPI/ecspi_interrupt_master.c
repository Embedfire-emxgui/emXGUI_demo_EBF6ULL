#include "BSP.h"
#include "fsl_ecspi.h"
#include "fsl_ecspi_sdma.h"

/*=========================================================================================*/

#define	PRINTF(x,...)

#define EXAMPLE_ECSPI_MASTER_BASEADDR 	ECSPI2
#define EXAMPLE_ECSPI_DEALY_COUNT 		1000000
#define ECSPI_MASTER_CLK_FREQ 			(CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 8U)
#define TRANSFER_SIZE 					64U         /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 				500000U /*! Transfer baudrate - 500k */

/*=========================================================================================*/

static uint32_t masterRxData[TRANSFER_SIZE] = {0U};
static uint32_t masterTxData[TRANSFER_SIZE] = {0U};

static ecspi_master_handle_t g_m_handle;
static volatile bool isTransferCompleted = false;

/*=========================================================================================*/

static void ECSPI_MasterUserCallback(ECSPI_Type *base, ecspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted = true;
    }

    if (status == kStatus_ECSPI_HardwareOverFlow)
    {
        PRINTF("Hardware overflow occured in this transfer. \r\n");
    }
}

static void ecspi2_isr(void)
{
	mk_isr_enter();
	ECSPI2_DriverIRQHandler();
	mk_isr_exit();
}
/*=========================================================================================*/

void ECSPI_Interrupt_MasterTest(void)
{

	ECSPI2_PortInit();

    CLOCK_SetMux(kCLOCK_EcspiMux, 0); /* Set ECSPI source to PLL3 60M */
    CLOCK_SetDiv(kCLOCK_EcspiDiv, 0); /* Set ECSPI divider to 1 */

    PRINTF("ECSPI board to board interrupt example.\r\n");
    PRINTF("This example use one board as master and another as slave.\r\n");
    PRINTF("Master and slave uses interrupt way. Slave should start first. \r\n");
    PRINTF("Please make sure you make the correct line connection. Basically, the connection is: \r\n");
    PRINTF("ECSPI_master -- ECSPI_slave   \r\n");
    PRINTF("   CLK      --    CLK  \r\n");
    PRINTF("   PCS      --    PCS \r\n");
    PRINTF("   MOSI     --    MOSI \r\n");
    PRINTF("   MISO     --    MISO \r\n");
    PRINTF("   GND      --    GND \r\n");

    DisableIRQ(eCSPI2_IRQn);
    SetIRQHandler(eCSPI2_IRQn,ecspi2_isr);
    EnableIRQ(eCSPI2_IRQn);


    uint32_t errorCount;
    uint32_t loopCount = 1;
    uint32_t i;
    ecspi_master_config_t masterConfig;
    ecspi_transfer_t masterXfer;


    /* Master config:
     * masterConfig.channel = kECSPI_Channel0;
     * masterConfig.burstLength = 8;
     * masterConfig.samplePeriodClock = kECSPI_spiClock;
     * masterConfig.baudRate_Bps = TRANSFER_BAUDRATE;
     * masterConfig.chipSelectDelay = 0;
     * masterConfig.samplePeriod = 0;
     * masterConfig.txFifoThreshold = 1;
     * masterConfig.rxFifoThreshold = 0;
     */
    ECSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = TRANSFER_BAUDRATE;

    ECSPI_MasterInit(EXAMPLE_ECSPI_MASTER_BASEADDR, &masterConfig, ECSPI_MASTER_CLK_FREQ);

    ECSPI_MasterTransferCreateHandle(EXAMPLE_ECSPI_MASTER_BASEADDR, &g_m_handle, ECSPI_MasterUserCallback, NULL);

    while (1)
    {
        /* Set up the transfer data */
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            masterTxData[i] = (i + loopCount) % 256U;
            masterRxData[i] = 0U;
        }

        /* Print out transmit buffer */
        PRINTF("\r\n Master transmit:\r\n");
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            /* Print 16 numbers in a line */
            if ((i & 0x0FU) == 0U)
            {
                PRINTF("\r\n");
            }
            PRINTF(" %02X", masterTxData[i]);
        }
        PRINTF("\r\n");

        /* Start master transfer, send data to slave */
        isTransferCompleted = false;
        masterXfer.txData = masterTxData;
        masterXfer.rxData = NULL;
        masterXfer.dataSize = TRANSFER_SIZE;
        masterXfer.channel = kECSPI_Channel0;
        ECSPI_MasterTransferNonBlocking(EXAMPLE_ECSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);

        /* Wait transfer complete */
        while (!isTransferCompleted)
        {
        }

        /* Delay to wait slave is ready */
        for (i = 0; i < EXAMPLE_ECSPI_DEALY_COUNT; i++)
        {
            __NOP();
        }

        /* Start master transfer, receive data from slave */
        isTransferCompleted = false;
        masterXfer.txData = NULL;
        masterXfer.rxData = masterRxData;
        masterXfer.dataSize = TRANSFER_SIZE;
        masterXfer.channel = kECSPI_Channel0;
        ECSPI_MasterTransferNonBlocking(EXAMPLE_ECSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);

        while (!isTransferCompleted)
        {
        }

        errorCount = 0U;
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            if (masterTxData[i] != masterRxData[i])
            {
                errorCount++;
            }
        }
        if (errorCount == 0U)
        {
            PRINTF(" \r\nECSPI transfer all data matched! \r\n");
            /* Print out receive buffer */
            PRINTF("\r\n Master received:\r\n");
            for (i = 0U; i < TRANSFER_SIZE; i++)
            {
                /* Print 16 numbers in a line */
                if ((i & 0x0FU) == 0U)
                {
                    PRINTF("\r\n");
                }
                PRINTF(" %02X", masterRxData[i]);
            }
            PRINTF("\r\n");
        }
        else
        {
            PRINTF(" \r\nError occured in ECSPI transfer ! \r\n");
        }

        /* Wait for press any key */
        PRINTF("\r\n Press any key to run again\r\n");
        //GETCHAR();
        SYS_msleep(100);
        /* Increase loop count to change transmit buffer */
        loopCount++;
    }
}

/*=========================================================================================*/
/*=========================================================================================*/
