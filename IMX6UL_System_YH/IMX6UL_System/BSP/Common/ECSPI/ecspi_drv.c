#include "BSP.h"
#include "fsl_ecspi.h"
#include "fsl_ecspi_sdma.h"

/*=========================================================================================*/
/*=========================================================================================*/

void	ECSPI1_PortInit(void)
{
	u32 pin_cfg =0x10B0;

	IOMUXC_MUX_CSI->DATA04.U =3; //ESPI1.SCLK
	IOMUXC_CFG_CSI->DATA04.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA05.U =3; //ESPI1.SS0
	IOMUXC_CFG_CSI->DATA05.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA06.U =3; //ESPI1.MOSI
	IOMUXC_CFG_CSI->DATA06.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA07.U =3; //ESPI1.MISO
	IOMUXC_CFG_CSI->DATA07.U =pin_cfg;

}


void	ECSPI2_PortInit(void)
{
	u32 pin_cfg =0x10B0;

	IOMUXC_MUX_CSI->DATA00.U =3; //ESPI2.SCLK
	IOMUXC_CFG_CSI->DATA00.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA01.U =3; //ESPI2.SS0
	IOMUXC_CFG_CSI->DATA01.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA02.U =3; //ESPI2.MOSI
	IOMUXC_CFG_CSI->DATA02.U =pin_cfg;

	IOMUXC_MUX_CSI->DATA03.U =3; //ESPI2.MISO
	IOMUXC_CFG_CSI->DATA03.U =pin_cfg;

}

/*=========================================================================================*/
/*=========================================================================================*/
