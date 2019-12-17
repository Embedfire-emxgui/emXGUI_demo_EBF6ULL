

#include "bsp.h"
#include "fsl_common.h"
#include "fsl_iomux.h"

/*============================================================================*/


#define	SAI_PIN_CFG		0x01B0B0u
//#define	SAI_PIN_CFG		pin_cfg

void SAI1_PortInit(void)
{

//	u32 pin_cfg;
//	u32 clk_pin_cfg;

	IOMUXC_MUX_CSI_t *csi_mux=IOMUXC_MUX_CSI;
	IOMUXC_CFG_CSI_t *csi_cfg=IOMUXC_CFG_CSI;


	csi_mux->DATA01.U =6; //MCLK
	csi_cfg->DATA01.U =SAI_PIN_CFG;

	csi_mux->DATA02.U =6; //RX_SYNC
	csi_cfg->DATA02.U =SAI_PIN_CFG;

	csi_mux->DATA03.U =6; //RX_BCLK
	csi_cfg->DATA03.U =SAI_PIN_CFG;

	csi_mux->DATA04.U =6; //TX_SYNC
	csi_cfg->DATA04.U =SAI_PIN_CFG;

	csi_mux->DATA05.U =6; //TX_BCLK
	csi_cfg->DATA05.U =SAI_PIN_CFG;

	csi_mux->DATA06.U =6; //RX_DATA
	csi_cfg->DATA06.U =SAI_PIN_CFG;

	csi_mux->DATA07.U =6; //TX_DATA
	csi_cfg->DATA07.U =SAI_PIN_CFG;



}

/*============================================================================*/

/*============================================================================*/
