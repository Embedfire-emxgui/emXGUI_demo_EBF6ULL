

#include "bsp.h"
#include "fsl_common.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

const clock_arm_pll_config_t g_ccmConfigArmPll_1056M = {.loopDivider = 88U};
const clock_arm_pll_config_t g_ccmConfigArmPll_1440M = {.loopDivider = 120U};
const clock_arm_pll_config_t g_ccmConfigArmPll_792M = {.loopDivider = 66U};
const clock_arm_pll_config_t g_ccmConfigArmPll_900M = {.loopDivider = 75U};

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void BOARD_BootClockGate(void)
{
    /* Disable all unused peripheral clock */
    CCM->CCGR0 = 0x00C0000FU;
    CCM->CCGR1 = 0xF00C0000U;
    CCM->CCGR2 = 0x003F0030U;
    CCM->CCGR3 = 0xFF3CC300U;
    CCM->CCGR4 = 0x0000F3FCU;
    CCM->CCGR5 = 0x00330033U;
    CCM->CCGR6 = 0x00CC0300U;
}

void BOARD_SetRunClock(void);

void BOARD_BootClockRUN(void)
{
    /* Boot ROM did initialize the XTAL, here we only sets external XTAL OSC freq */
    CLOCK_SetXtalFreq(24000000U);
    CLOCK_SetRtcXtalFreq(32768U);

    /* Switch CPU off ARM PLL */
    if (CLOCK_GetMux(kCLOCK_Pll1SwMux) == 0) /* CPU runs on ARM PLL */
    {
        CLOCK_SetMux(kCLOCK_StepMux, 0);   /* Set Step MUX to OSC */
        CLOCK_SetMux(kCLOCK_Pll1SwMux, 1); /* Let CPU run on Step MUX */
    }
#if (defined(CPU_MCIMX6Y2CVM05) || defined(CPU_MCIMX6Y2DVM05) || defined(CPU_MCIMX6Y0CVM05) || \
     defined(CPU_MCIMX6Y0DVM05) || defined(CPU_MCIMX6Y1CVM05) || defined(CPU_MCIMX6Y1CVK05) || \
     defined(CPU_MCIMX6Y1DVM05) || defined(CPU_MCIMX6Y1DVK05) || defined(CPU_MCIMX6Y7DVK05))
    CLOCK_InitArmPll(&g_ccmConfigArmPll_1056M); /* Configure ARM PLL to 1056M */
    CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);          /* Now CPU runs again on ARM PLL*/
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1);             /* Configure ARM clock root with divide 2 */
#elif defined(CPU_MCIMX6Y2CVM08)
    CLOCK_InitArmPll(&g_ccmConfigArmPll_792M); /* Configure ARM PLL to 792M */
    CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);         /* Now CPU runs again on ARM PLL*/
    CLOCK_SetDiv(kCLOCK_ArmDiv, 0);            /* Configure ARM clock root with divide 1 */
#elif(defined(CPU_MCIMX6Y2DVM09) || defined(CPU_MCIMX6Y7DVM09))
    CLOCK_InitArmPll(&g_ccmConfigArmPll_900M); /* Configure ARM PLL to 900M */
    CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);         /* Now CPU runs again on ARM PLL*/
    CLOCK_SetDiv(kCLOCK_ArmDiv, 0);            /* Configure ARM clock root with divide 1 */
#endif

    CLOCK_SetMux(kCLOCK_AxiAltMux, 0); //AXI clock = PLL2_PFD2
    CLOCK_SetDiv(kCLOCK_AxiDiv, 1);

    CLOCK_SetDiv(kCLOCK_AhbDiv, 1);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 2);
    CLOCK_SetDiv(kCLOCK_FabricMmdcDiv, 0);

    //BOARD_SetRunClock();

    /* Disable unused clock */
    BOARD_BootClockGate();

    /* Power down all unused PLL */
    CLOCK_DeinitUsb2Pll();
    CLOCK_DeinitAudioPll();
    CLOCK_DeinitVideoPll();
    CLOCK_DeinitEnetPll();

    /* Configure UART divider to default */
    CLOCK_SetMux(kCLOCK_UartMux, 0); /* Set UART source to PLL3 80M */
    CLOCK_SetDiv(kCLOCK_UartDiv, 0); /* Set UART divider to 1 */

    CLOCK_SetMux(kCLOCK_PerclkMux, 0); /* Set I2C/EPIT source to IPG 66M */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0); /* Set I2C/EPIT divider to 1 */

    /* Update core clock */
 //   SystemCoreClockUpdate();
}
