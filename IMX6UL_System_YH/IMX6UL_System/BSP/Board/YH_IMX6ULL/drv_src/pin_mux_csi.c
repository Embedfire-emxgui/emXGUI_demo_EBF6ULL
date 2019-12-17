

#include "bsp.h"
#include "fsl_common.h"
#include "fsl_iomux.h"

/*============================================================================*/

#if 0
	pin_cfg =
			IOMUXC_SW_PAD_CTL_PAD_SPEED(CARD_BUS_FREQ_100MHZ1) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
            IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |
            IOMUXC_SW_PAD_CTL_PAD_DSE(CARD_BUS_STRENGTH_7);

	clk_pin_cfg =
			IOMUXC_SW_PAD_CTL_PAD_SPEED(CARD_BUS_FREQ_100MHZ1) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
	        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
	        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(0) |   \
	        IOMUXC_SW_PAD_CTL_PAD_DSE(CARD_BUS_STRENGTH_7);
#endif

#define	CSI_PIN_MUX		0
#define	CSI_PIN_CFG		0x01B0B0u
//#define	CSI_PIN_CFG		pin_cfg

void CSI_PortInit(void)
{
//	u32 pin_cfg;
//	u32 clk_pin_cfg;

	IOMUXC_MUX_CSI_t *csi_mux=IOMUXC_MUX_CSI;
	IOMUXC_CFG_CSI_t *csi_cfg=IOMUXC_CFG_CSI;

	PWR_PinInit();


	csi_mux->DATA00.U =CSI_PIN_MUX;
	csi_cfg->DATA00.U =CSI_PIN_CFG;

	csi_mux->DATA01.U =CSI_PIN_MUX;
	csi_cfg->DATA01.U =CSI_PIN_CFG;

	csi_mux->DATA02.U =CSI_PIN_MUX;
	csi_cfg->DATA02.U =CSI_PIN_CFG;

	csi_mux->DATA03.U =CSI_PIN_MUX;
	csi_cfg->DATA03.U =CSI_PIN_CFG;

	csi_mux->DATA04.U =CSI_PIN_MUX;
	csi_cfg->DATA04.U =CSI_PIN_CFG;

	csi_mux->DATA05.U =CSI_PIN_MUX;
	csi_cfg->DATA05.U =CSI_PIN_CFG;

	csi_mux->DATA06.U =CSI_PIN_MUX;
	csi_cfg->DATA06.U =CSI_PIN_CFG;

	csi_mux->DATA07.U =CSI_PIN_MUX;
	csi_cfg->DATA07.U =CSI_PIN_CFG;

	csi_mux->HSYNC.U =CSI_PIN_MUX;
	csi_cfg->HSYNC.U =CSI_PIN_CFG;

	csi_mux->VSYNC.U =CSI_PIN_MUX;
	csi_cfg->VSYNC.U =CSI_PIN_CFG;

	csi_mux->PCLK.U =CSI_PIN_MUX;
	csi_cfg->PCLK.U =CSI_PIN_CFG;

	csi_mux->MCLK.U =CSI_PIN_MUX;
	csi_cfg->MCLK.U =CSI_PIN_CFG;



#if 0
	rIOMUXC_MUX(MUX_SD1_CMD).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_CMD).U =pin_cfg;

	rIOMUXC_MUX(MUX_SD1_CLK).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_CLK).U =clk_pin_cfg;

	rIOMUXC_MUX(MUX_SD1_DAT0).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_DAT0).U =pin_cfg;

	rIOMUXC_MUX(MUX_SD1_DAT1).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_DAT1).U =pin_cfg;

	rIOMUXC_MUX(MUX_SD1_DAT2).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_DAT2).U =pin_cfg;

	rIOMUXC_MUX(MUX_SD1_DAT3).U =CSI_PIN_MUX;
	rIOMUXC_CFG(CFG_SD1_DAT3).U =pin_cfg;
#endif

}

/*============================================================================*/

void BOARD_PullCameraResetPin(bool pullUp)
{
    if (pullUp)
    {
        //BOARD_NXP74LV595_SetValue(kNXP74LV595_CSI_RST, kSignal_NXP74LV595_High);
    	HC595_SetPin(5,1);
    }
    else
    {
        //BOARD_NXP74LV595_SetValue(kNXP74LV595_CSI_RST, kSignal_NXP74LV595_Low);
    	HC595_SetPin(5,0);
    }
}

void BOARD_PullCameraPowerDownPin(bool pullUp)
{
    if (pullUp)
    {
        //BOARD_NXP74LV595_SetValue(kNXP74LV595_CSI_PWDN, kSignal_NXP74LV595_High);
    	HC595_SetPin(6,1);
    }
    else
    {
        //BOARD_NXP74LV595_SetValue(kNXP74LV595_CSI_PWDN, kSignal_NXP74LV595_Low);
    	HC595_SetPin(6,0);
    }
}

/*============================================================================*/
