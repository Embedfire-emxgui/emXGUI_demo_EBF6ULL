#include	"BSP.h"


/*==================================================================*/

void Undef_Handler_C(void)
{
	//DebugPuts("Undef.\r\n");

}

void PaAbort_Handler_C(void)
{
	//DebugPuts("PaAbort.\r\n");
}

void DaAbort_Handler_C(void)
{
	//DebugPuts("DaAbort.\r\n");

}


/*==================================================================*/


static 	void init_ddr(void)
{
	/* New DDR type MT41K256M16TW-107 */
	/* Enable all clocks */
    write32( 0x020c4068 , 0xffffffff);
    write32( 0x020c406c , 0xffffffff);
    write32( 0x020c4070 , 0xffffffff);
    write32( 0x020c4074 , 0xffffffff);
    write32( 0x020c4078 , 0xffffffff);
    write32( 0x020c407c , 0xffffffff);
    write32( 0x020c4080 , 0xffffffff);

    write32( 0x020E04B4 , 0x000C0000);
    write32( 0x020E04AC , 0x00000000);
    write32( 0x020E027C , 0x00000030);
    write32( 0x020E0250 , 0x00000030);
    write32( 0x020E024C , 0x00000030);
    write32( 0x020E0490 , 0x00000030);
    write32( 0x020E0288 , 0x00000030);
    write32( 0x020E0270 , 0x00000000);
    write32( 0x020E0260 , 0x00000030);
    write32( 0x020E0264 , 0x00000030);
    write32( 0x020E04A0 , 0x00000030);
    write32( 0x020E0494 , 0x00020000);
    write32( 0x020E0280 , 0x00000030);
    write32( 0x020E0284 , 0x00000030);
    write32( 0x020E04B0 , 0x00020000);
    write32( 0x020E0498 , 0x00000030);
    write32( 0x020E04A4 , 0x00000030);
    write32( 0x020E0244 , 0x00000030);
    write32( 0x020E0248 , 0x00000030);
    write32( 0x021B001C , 0x00008000);
    write32( 0x021B0800 , 0xA1390003);
    write32( 0x021B080C , 0x00000000);
    write32( 0x021B083C , 0x41570155);
    write32( 0x021B0848 , 0x4040474A);
    write32( 0x021B0850 , 0x40405550);
    write32( 0x021B081C , 0x33333333);
    write32( 0x021B0820 , 0x33333333);
    write32( 0x021B082C , 0xf3333333);
    write32( 0x021B0830 , 0xf3333333);
    write32( 0x021B08C0 , 0x00921012);
    write32( 0x021B08b8 , 0x00000800);
    write32( 0x021B0004 , 0x0002002D);
    write32( 0x021B0008 , 0x1B333030);
    write32( 0x021B000C , 0x676B52F3);
    write32( 0x021B0010 , 0xB66D0B63);
    write32( 0x021B0014 , 0x01FF00DB);
    write32( 0x021B0018 , 0x00201740);
    write32( 0x021B001C , 0x00008000);
    write32( 0x021B002C , 0x000026D2);
    write32( 0x021B0030 , 0x006B1023);
    write32( 0x021B0040 , 0x0000004F);
    write32( 0x021B0000 , 0x84180000);
    write32( 0x021B0890 , 0x23400A38);
    write32( 0x021B001C , 0x02008032);
    write32( 0x021B001C , 0x00008033);
    write32( 0x021B001C , 0x00048031);
    write32( 0x021B001C , 0x15208030);
    write32( 0x021B001C , 0x04008040);
    write32( 0x021B0020 , 0x00000800);
    write32( 0x021B0818 , 0x00000227);
    write32( 0x021B0004 , 0x0002552D);
    write32( 0x021B0404 , 0x00011006);
    write32( 0x021B001C , 0x00000000);
}


#define	MMDC_BASE	0x021B0000

#define GPIO5_PA_BASE    0x020AC000
#define GPIO5_PA         ((GPIO_Type *)GPIO5_PA_BASE)

#define CCM_PA_BASE      0x020C4000
#define CCM_PA           ((CCM_Type *)CCM_PA_BASE)

#define	IOMUX_REG_PA_BASE	0x020E0000

#define	rpIOMUXC_MUX(offset)				(*(volatile IOMUXC_MUX_t*)(IOMUX_REG_PA_BASE + offset))
#define	rpIOMUXC_CFG(offset)				(*(volatile IOMUXC_CFG_t*)(IOMUX_REG_PA_BASE + offset))

static inline void _CLOCK_ControlGate(clock_ip_name_t name, clock_gate_value_t value)
{
    uint32_t index = ((uint32_t)name) >> 8;
    uint32_t shift = (((uint32_t)name) & 0xF) << 1;
    volatile uint32_t *reg;

 //   assert (index <= 6);

    reg = ((volatile uint32_t *)&CCM_PA->CCGR0) + index;
    *reg = ((*reg) & ~(3U << shift)) | (((uint32_t)value) << shift);
}

static void _GPIO_PinWrite(GPIO_Type *base, uint32_t pin, uint8_t output)
{
    assert(pin < 32);
    if (output == 0U)
    {
        base->DR &= ~(1U << pin); /* Set pin output to low level.*/
    }
    else
    {
        base->DR |= (1U << pin); /* Set pin output to high level.*/
    }
}

static void _GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *Config)
{

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable GPIO clock. */
  //  CLOCK_EnableClock(s_gpioClock[GPIO_GetInstance(base)]);
    _CLOCK_ControlGate(kCLOCK_Gpio5,kCLOCK_ClockNeededRunWait);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Register reset to default value */
    base->IMR &= ~(1U << pin);

    /* Configure GPIO pin direction */
    if (Config->direction == kGPIO_DigitalInput)
    {
        base->GDIR &= ~(1U << pin);
    }
    else
    {
        _GPIO_PinWrite(base, pin, Config->outputLogic);
        base->GDIR |= (1U << pin);
    }

    /* Configure GPIO pin interrupt mode */
   // GPIO_SetPinInterruptConfig(base, pin, Config->interruptMode);
}

void led_test_nommu(int t)
{
	//volatile int i;
	gpio_pin_config_t cfg;
	volatile int i;

	IOMUXC_MUX_SNVS_t *iomux =(IOMUXC_MUX_SNVS_t*)IOMUXC_MUX_SNVS_PA_BASE;
	IOMUXC_CFG_SNVS_t *iocfg =(IOMUXC_CFG_SNVS_t*)IOMUXC_CFG_SNVS_PA_BASE;

	iomux->TAMPER3.U =0x5;
	iocfg->TAMPER3.U =0x10B0;
	//rpIOMUXC_MUX(MUX_SNVS_TAMPER3).U =0x05;
	//rpIOMUXC_CFG(CFG_SNVS_TAMPER3).U =0x10B0;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;

	_GPIO_PinInit(GPIO5_PA,3,&cfg);
	while(t-- > 0)
	{
		_GPIO_PinWrite(GPIO5_PA,3,1);
		for(i=0;i<0x7FFFF;i++);
		_GPIO_PinWrite(GPIO5_PA,3,0);
		for(i=0;i<0x7FFFF;i++);

	}

}


void  system_init_nommu(register U32 pc)
{

	u32 addr;

	led_test_nommu(3);

	addr =(u32)&system_init_nommu;

	if(addr<0x80000000 || addr>0x90000000)
	{
		//init_ddr();
	}

	// Disable all interrupts.
//	reg32(REG_AIC_MDCR) =0xFFFFFFFE;
//	reg32(REG_AIC_MDCRH) =0x3FFFFFFF;

	KernelMemMap_Init((u32*)MMU_TT_PA_BASE);

	mmu_set_domain(0xFFFFFFFF);
	mmu_set_mtt_base(MMU_TT_PA_BASE);

#if 1
	uint32_t sctlr;
	ACTLR_Type actlr;
	L1C_InvalidateInstructionCacheAll();
	L1C_InvalidateDataCacheAll();

	actlr.w = __get_ACTLR();
	actlr.b.SMP =1;  /* Change to SMP mode before enable DCache */
//	actlr.b.L1PCTL =2;
	//actlr.b.DDI =1;
	//actlr.b.DDVM =1;

	//actlr = (actlr | ACTLR_SMP_Msk);
	__set_ACTLR(actlr.w);
#endif

#if 0
	  sctlr = __get_SCTLR();
	  sctlr = (sctlr & ~(SCTLR_V_Msk       | /* Use low vector */
	                     SCTLR_A_Msk       | /* Disable alignment fault checking */
	                     SCTLR_M_Msk))       /* Disable MMU */
	                 |  (SCTLR_I_Msk       | /* Enable ICache */
	                     SCTLR_Z_Msk       | /* Enable Prediction */
	                     SCTLR_CP15BEN_Msk | /* Enable CP15 barrier operations */
	                     SCTLR_C_Msk);       /* Enable DCache */
	  __set_SCTLR(sctlr);
#endif

}

/*==================================================================*/

static void SDMA_IRQHandler(void)
{
	SDMA_DriverIRQHandler();
}

/*==================================================================*/

volatile u32 clkArmPLL,clkSysPLL,clkAHB,clkIPG,clkAXI,clkMMDC;
volatile u32 SysPllClkPfd0,SysPllClkPfd1,SysPllClkPfd2,SysPllClkPfd3;
volatile u32 clkGPIO1;
void BOARD_BootClockRUN(void);
void gic_init(void);

void system_init(void)
{
	BOARD_BootClockRUN();

	clkArmPLL =CLOCK_GetFreq(kCLOCK_ArmPllClk);
	clkSysPLL =CLOCK_GetFreq(kCLOCK_SysPllClk);

	SysPllClkPfd0=CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk);
	SysPllClkPfd1=CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk);
	SysPllClkPfd2=CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk);
	SysPllClkPfd3=CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk);

	clkAHB =CLOCK_GetFreq(kCLOCK_AhbClk);
	clkIPG =CLOCK_GetFreq(kCLOCK_IpgClk);
	clkAXI =CLOCK_GetFreq(kCLOCK_AxiClk);
	clkMMDC =CLOCK_GetFreq(kCLOCK_MmdcClk);


	clkGPIO1=CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk);


	gic_init();
	SetIRQHandler(SDMA_IRQn,SDMA_IRQHandler);
	EnableIRQ(SDMA_IRQn);

//	CLOCK_EnableClock(kCLOCK_Aips_tz1);
//	CLOCK_EnableClock(kCLOCK_Aips_tz2);
//	CLOCK_EnableClock(kCLOCK_Semc);
	CLOCK_EnableClock(kCLOCK_Iomuxc);
	CLOCK_EnableClock(kCLOCK_IomuxcGpr);
	CLOCK_EnableClock(kCLOCK_Ipmux1);
	CLOCK_EnableClock(kCLOCK_Ipmux2);
	CLOCK_EnableClock(kCLOCK_Ipmux3);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);
	CLOCK_EnableClock(kCLOCK_Gpio3);
	CLOCK_EnableClock(kCLOCK_Gpio4);
	CLOCK_EnableClock(kCLOCK_Gpio5);


}


/*==================================================================*/


