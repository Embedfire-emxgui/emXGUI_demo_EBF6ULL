
#include "fsl_clock.h"
#include "fsl_iomux.h"


/*=========================================================================================*/

//#define	IOMUXC_UART1_TX	0x120,0x340,0,5
//#define	rIOMUXC(r_mux,r_cfg,mux,cfg)	(*(volatile u32*)(IOMUX_REG_BASE + r_mux))=mux; (*(volatile u32*)(IOMUX_REG_BASE + r_cfg))=cfg;


//#define	IOMUXC_Config(offset,mux) (*(volatile IOMUXC_MUX_t*)(IOMUX_REG_BASE + offset)).B.MUX =mux;


static void test(void)
{
	u32 c;



	//IOMUXC_Config(IOMUXC_GPIO1_IO08_PWM1OUT,0x10B0);
	//IOMUXC_Config(0x07C,0);

/*
	c = (u32)&rIOMUXC_MUX(MUX_UART5_TX);

	rIOMUXC_MUX(MUX_UART1_TX).B.MUX =0;
	rIOMUXC_CFG(CFG_UART1_TX).B.PUE =0;

	rIOMUXC_MUX_CFG(UART1_TX,0,0x10B0u);
*/

}

/*=========================================================================================*/
/*=========================================================================================*/

