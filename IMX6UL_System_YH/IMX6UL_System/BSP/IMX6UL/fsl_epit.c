/*
 * imx_epit.c
 *
 *  Created on: 2016Äê3ÔÂ17ÈÕ
 *      Author: liuwei
 */

#include "def.h"
#include "fsl_clock.h"
#include "fsl_epit.h"
#include "interrupt.h"

#define	EPIT_PRESCALE	2
#define	EPIT_CLKSRC		CLKSRC_PER_CLK //IPG Clk

#define	EPIT_CLKFIN		(CLOCK_GetFreq(kCLOCK_IpgClk)/2)

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static const u32 s_epitBase[]=
{

	regEPIT1_BASE,
	regEPIT2_BASE,
};

static const u8 epit_irq_id[2]={

	IMX_INT_EPIT1,
	IMX_INT_EPIT2,
};

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
static const clock_ip_name_t s_epitClocks[] = {kCLOCK_Epit1,kCLOCK_Epit2};//EPIT_CLOCKS;
#endif

static uint32_t EPIT_GetInstance(u32 base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0U; instance < ARRAY_SIZE(s_epitBase); instance++)
    {
        if (s_epitBase[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_epitBase));

    return instance;
}


void epit_reload_counter(u32 epit_ch, u32 load_val)
{
	u32 reg_base=s_epitBase[epit_ch];

    // set the load register especially if RLD=reload_mode=SET_AND_FORGET=1
	write32(reg_base+EPIT_LR,load_val);
}

u32 epit_get_counter_value(u32 epit_ch)
{
	u32 reg_base=s_epitBase[epit_ch];

    return read32(reg_base+EPIT_CNR);
}

void epit_set_compare_event(u32 epit_ch, u32 compare_val)
{
	u32 reg_base=s_epitBase[epit_ch];

	write32(reg_base+EPIT_CMPR,compare_val);
}

void epit_irq_clean(u32 epit_ch)
{
	u32 reg_base=s_epitBase[epit_ch];
    u32 sr;

    // get the status
    sr = read32(reg_base+EPIT_SR);

    // clear it if found set
    if (sr & (1<<0))
    {
    	write32(reg_base+EPIT_SR,sr);
    }

}

void epit_stop(u32 epit_ch)
{
	u32 reg_base=s_epitBase[epit_ch];

    /* temporary workaround for the discovered issue when disabling the
     * counter during end of count/reload/set compare flag ??.
     * Set to the max value so that it ensures that the counter couldn't
     * reach 0 when it is disabled.
     */
    rEPIT_LR(reg_base).U = 0xFFFFFFFF;

    // disable the counter
    rEPIT_CR(reg_base).B.EN =0;

    // ensure to leave the counter in a proper state
    // by disabling the output compare interrupt
    rEPIT_CR(reg_base).B.OCIEN =0;

    // and clearing possible remaining compare event
    rEPIT_SR(reg_base).B.OCIF =1;
}

void epit_start(u32 epit_ch,bool irq_mode)
{
	u32 reg_base=s_epitBase[epit_ch];


    // ensure to start the counter in a proper state
    // by clearing possible remaining compare event
    rEPIT_SR(reg_base).B.OCIF =1;

    // set the mode when the output compare event occur: IRQ or polling
    if (irq_mode)
    {
        rEPIT_CR(reg_base).B.OCIEN =1;
    }
    else
    {
        // polling
    	rEPIT_CR(reg_base).B.OCIEN =0;
    }

    // finally, enable the counter
    rEPIT_CR(reg_base).B.EN =1;
}

void epit_setup_interrupt(u32 epit_ch, void (*handler)(void), bool enableIt)
{
    u32 irq_id = epit_irq_id[epit_ch];

    if (enableIt)
    {
        // register the IRQ sub-routine
        register_interrupt_routine(irq_id, handler);

        // enable the IRQ
        enable_interrupt(irq_id, CPU_0, 0);
    }
    else
    {
        // disable the IRQ
        disable_interrupt(irq_id, CPU_0);
    }
}

void epit_init(u32 epit_ch,u32 hz,bool reload_mode,  u32 low_power_mode)
{
    u32 control_reg_tmp = 0;
    u32 base = s_epitBase[epit_ch];

    // enable the source clocks to the EPIT port
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the GPT clock*/
    CLOCK_EnableClock(s_epitClocks[EPIT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    //clock_gating_config(SFR_VA2PA(base), CLOCK_ON);

    // start with a known state by disabling and reseting the module
    //HW_EPIT_CR_WR(epit_ch, BM_EPIT_CR_SWR);
    rEPIT_CR(base).B.SWR =1;

    // wait for the reset to complete
    while (rEPIT_CR(base).B.SWR != 0) ;

    rEPIT_CR(base).B.EN =0;

    // set the reference source clock for the counter
    rEPIT_CR(base).B.CLKSRC =EPIT_CLKSRC;

    // set the counter clock prescaler value - 0 to 4095
    rEPIT_CR(base).B.PRESCALAR =EPIT_PRESCALE-1;

    // set the reload mode
    if (reload_mode)
    {
    	rEPIT_CR(base).B.RLD =1;
    }
    else
    {
    	rEPIT_CR(base).B.RLD =0;
    }

    // set behavior for low power mode
    if (low_power_mode & WAIT_MODE_EN)
    {
        rEPIT_CR(base).B.WAITEN =1;
    }
    if (low_power_mode & STOP_MODE_EN)
    {
        rEPIT_CR(base).B.STOPEN =1;
    }

    // make the counter start from a known value when enabled, this is loaded from
    // EPITLR register if RLD=reload_mode=1 or 0xFFFFFFFF if RLD=reload_mode=0
    rEPIT_CR(base).B.IOVW =1;
    rEPIT_CR(base).B.ENMOD =1;

    // finally write the control register
   // HW_EPIT_CR_WR(epit_ch, control_reg_tmp);

    // initialize the load register especially if RLD=reload_mode=SET_AND_FORGET=1
    // and if the value is different from 0 which is the lowest counter value

    rEPIT_LR(base).U =EPIT_CLKFIN/hz;



}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////


