#include "BSP.h"
#include "fsl_ecspi.h"
#include "fsl_ecspi_sdma.h"

/*=========================================================================================*/

#define	PRINTF(x,...)

#define EXAMPLE_ECSPI_SLAVE_BASEADDR ECSPI2
#define TRANSFER_SIZE 64U /*! Transfer dataSize */

/*=========================================================================================*/

static uint32_t slaveRxData[TRANSFER_SIZE] = {0U};
static ecspi_slave_handle_t g_s_handle;

static volatile bool isTransferCompleted = false;

/*=========================================================================================*/

static void ECSPI_SlaveUserCallback(ECSPI_Type *base, ecspi_slave_handle_t *handle, status_t status, void *userData)
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

void ECSPI_Interrupt_SlaveTest(void)
{

    CLOCK_SetMux(kCLOCK_EcspiMux, 0); /* Set ECSPI source to PLL3 60M */
    CLOCK_SetDiv(kCLOCK_EcspiDiv, 0); /* Set ECSPI divider to 1 */

    PRINTF("ECSPI board to board interrupt example.\r\n");

    uint32_t i;
    ecspi_slave_config_t slaveConfig;
    ecspi_transfer_t slaveXfer;

    /* Slave config:
     * slaveConfig.channel = kECSPI_Channel0;
     * slaveConfig.burstLength = 8;
     * slaveConfig.txFifoThreshold = 1;
     * slaveConfig.rxFifoThreshold = 0;
     */
    ECSPI_SlaveGetDefaultConfig(&slaveConfig);
    ECSPI_SlaveInit(EXAMPLE_ECSPI_SLAVE_BASEADDR, &slaveConfig);

    ECSPI_SlaveTransferCreateHandle(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_s_handle, ECSPI_SlaveUserCallback, NULL);

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
        slaveXfer.txData = NULL;
        slaveXfer.rxData = slaveRxData;
        slaveXfer.dataSize = TRANSFER_SIZE;
        slaveXfer.channel = kECSPI_Channel0;

        /* Slave start receive */
        PRINTF("Slave starts to receive data!\r\n");
        ECSPI_SlaveTransferNonBlocking(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_s_handle, &slaveXfer);

        while (!isTransferCompleted)
        {
        }

        /* Set slave transfer ready to send back data */
        isTransferCompleted = false;
        slaveXfer.txData = slaveRxData;
        slaveXfer.rxData = NULL;
        slaveXfer.dataSize = TRANSFER_SIZE;
        slaveXfer.channel = kECSPI_Channel0;

        /* Slave start send */
        PRINTF("Slave starts to transmit data!\r\n");
        ECSPI_SlaveTransferNonBlocking(EXAMPLE_ECSPI_SLAVE_BASEADDR, &g_s_handle, &slaveXfer);

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
/*=========================================================================================*/
