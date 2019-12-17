
//#include <assert.h>

#include "bsp.h"
#include "fsl_common.h"
#include "gic.h"
#include "ARMv7/cortex_a9.h"

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////


//! @brief Offsets to the GIC registers.
enum _gic_base_offsets
{
    kGICDBaseOffset = 0x1000,    //!< GIC distributor offset.

	//kGICCBaseOffset = 0x0100,  //!< GIC CPU interface offset(IMX6Q,IMX6D).
    kGICCBaseOffset = 0x2000     //!< GIC CPU interface offset(IMX6UL).
};

//! @brief GIC distributor registers.
//!
//! Uses the GICv2 register names, but does not include GICv2 registers.
//!
//! The IPRIORITYRn and ITARGETSRn registers are byte accessible, so their types are uint8_t
//! instead of uint32_t to reflect this. These members are indexed directly with the interrupt
//! number.
struct _gicd_registers
{
	volatile uint32_t CTLR;              //!< Distributor Control Register.
	volatile uint32_t TYPER;             //!< Interrupt Controller Type Register.
	volatile uint32_t IIDR;              //!< Distributor Implementer Identification Register.
	volatile uint32_t _reserved0[29];
	volatile uint32_t IGROUPRn[8];       //!< Interrupt Group Registers.
	volatile uint32_t _reserved1[24];
	volatile uint32_t ISENABLERn[32];    //!< Interrupt Set-Enable Registers.
	volatile uint32_t ICENABLERn[32];    //!< Interrupt Clear-Enable Registers.
	volatile uint32_t ISPENDRn[32];      //!< Interrupt Set-Pending Registers.
	volatile uint32_t ICPENDRn[32];      //!< Interrupt Clear-Pending Registers.
	volatile uint32_t ICDABRn[32];       //!< Active Bit Registers.
	volatile uint32_t _reserved2[32];
	volatile uint8_t IPRIORITYRn[255 * sizeof(uint32_t)];  //!< Interrupt Priority Registers. (Byte accessible)
	volatile uint32_t _reserved3;
	volatile uint8_t ITARGETSRn[255 * sizeof(uint32_t)];   //!< Interrupt Processor Targets Registers. (Byte accessible)
	volatile uint32_t _reserved4;
	volatile uint32_t ICFGRn[64];        //!< Interrupt Configuration Registers.
	volatile uint32_t _reserved5[128];
	volatile uint32_t SGIR;              //!< Software Generated Interrupt Register
};

//! @brief Bitfields constants for the GICD_CTLR register.
enum _gicd_ctlr_fields
{
    kBM_GICD_CTLR_EnableGrp1 = (1 << 1),
    kBM_GICD_CTLR_EnableGrp0 = (1 << 0)
};

//! @brief Bitfields constants for the GICD_SGIR register.
enum _gicd_sgir_fields
{
    kBP_GICD_SGIR_TargetListFilter = 24,
    kBM_GICD_SGIR_TargetListFilter = (0x3 << kBP_GICD_SGIR_TargetListFilter),

    kBP_GICD_SGIR_CPUTargetList = 16,
    kBM_GICD_SGIR_CPUTargetList = (0xff << kBP_GICD_SGIR_CPUTargetList),

    kBP_GICD_SGIR_NSATT = 15,
    kBM_GICD_SGIR_NSATT = (1 << kBP_GICD_SGIR_NSATT),

    kBP_GICD_SGIR_SGIINTID = 0,
    kBM_GICD_SGIR_SGIINTID = 0xf
};

//! @brief GIC CPU interface registers.
//!
//! Uses the GICv2 register names. Does not include GICv2 registers.
struct _gicc_registers
{
	volatile uint32_t CTLR;  //!< CPU Interface Control Register.
	volatile uint32_t PMR;   //!< Interrupt Priority Mask Register.
	volatile uint32_t BPR;   //!< Binary Point Register.
	volatile uint32_t IAR;   //!< Interrupt Acknowledge Register.
	volatile uint32_t EOIR;  //!< End of Interrupt Register.
	volatile uint32_t RPR;   //!< Running Priority Register.
	volatile uint32_t HPPIR; //!< Highest Priority Pending Interrupt Register.
	volatile uint32_t ABPR;  //!< Aliased Binary Point Register. (only visible with a secure access)
	volatile uint32_t _rsv[56];
	volatile uint32_t IIDR;  //!< CPU Interface Identification Register.
};

//! @brief Bitfields constants for the GICC_CTLR register.
enum _gicc_ctlr_fields
{
    kBP_GICC_CTLR_EnableS = 0,
    kBM_GICC_CTLR_EnableS = (1 << 0),

    kBP_GICC_CTLR_EnableNS = 1,
    kBM_GICC_CTLR_EnableNS = (1 << 1),

    kBP_GICC_CTLR_AckCtl = 2,
    kBM_GICC_CTLR_AckCtl = (1 << 2),

    kBP_GICC_CTLR_FIQEn = 3,
    kBM_GICC_CTLR_FIQEn = (1 << 3),

    kBP_GICC_CTLR_SBPR = 4,
    kBM_GICC_CTLR_SBPR = (1 << 4)
};

//! @brier Type for the GIC distributor registers.
typedef struct _gicd_registers gicd_t;

//! @brier Type for the GIC CPU interface registers.
typedef struct _gicc_registers gicc_t;


static inline gicd_t * gic_get_gicd(void);
static inline gicc_t * gic_get_gicc(void);
static inline uint32_t irq_get_register_offset(uint32_t irqID);
static inline uint32_t irq_get_bit_offset(uint32_t irqID);
static inline uint32_t irq_get_bit_mask(uint32_t irqID);

#define	GIC_REG_BASE	GIC_VA_BASE

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static inline gicd_t * gic_get_gicd(void)
{
    uint32_t base;

    //base =get_arm_private_peripheral_base();
    base =SFR_PA2VA(base);
    base = GIC_REG_BASE;
    base += kGICDBaseOffset;

    return (gicd_t *)base;
}

static inline gicc_t * gic_get_gicc(void)
{

	uint32_t base;// = SFR_PA2VA(get_arm_private_peripheral_base());

	//base = get_arm_private_peripheral_base();
	//base = SFR_PA2VA(base);

	base =GIC_REG_BASE;
	//printf("private_peripheral_base = 0x%p\n", base);
	base += kGICCBaseOffset;
    return (gicc_t *)base;
}

static inline uint32_t irq_get_register_offset(uint32_t irqID)
{
    return irqID / 32;
}

static inline uint32_t irq_get_bit_offset(uint32_t irqID)
{
    return irqID & 0x1f;
}

static inline uint32_t irq_get_bit_mask(uint32_t irqID)
{
    return 1 << irq_get_bit_offset(irqID);    
}

void gic_enable(bool enableIt)
{
    gicd_t * gicd = gic_get_gicd();
    
    if (enableIt)
    {
        // Enable both secure and non-secure.
        gicd->CTLR |= kBM_GICD_CTLR_EnableGrp0 | kBM_GICD_CTLR_EnableGrp1;
    }
    else
    {
        // Clear the enable bits.
        gicd->CTLR &= ~(kBM_GICD_CTLR_EnableGrp0 | kBM_GICD_CTLR_EnableGrp1);
    }
}

void gic_set_irq_security(uint32_t irqID, bool isSecure)
{
    gicd_t * gicd = gic_get_gicd();
    
    uint32_t reg = irq_get_register_offset(irqID);
    uint32_t mask = irq_get_bit_mask(irqID);
    
    uint32_t value = gicd->IGROUPRn[reg];
    if (!isSecure)
    {
        value &= ~mask;
    }
    else
    {
        value |= mask;
    }
    gicd->IGROUPRn[reg] = value;
}

void gic_enable_irq(uint32_t irqID, bool isEnabled)
{
    gicd_t * gicd = gic_get_gicd();
    
    uint32_t reg = irq_get_register_offset(irqID);
    uint32_t mask = irq_get_bit_mask(irqID);
    
    // Select set-enable or clear-enable register based on enable flag.
    if (isEnabled)
    {
        gicd->ISENABLERn[reg] = mask;
    }
    else
    {
        gicd->ICENABLERn[reg] = mask;
    }
}

void gic_set_irq_priority(uint32_t ID, uint32_t priority)
{
    gicd_t * gicd = gic_get_gicd();
    
    // Update the priority register. The priority registers are byte accessible, and the register
    // struct has the priority registers as a byte array, so we can just index directly by the
    // interrupt ID.
    gicd->IPRIORITYRn[ID] = priority & 0xff;
}

void gic_set_cpu_target(uint32_t irqID, unsigned cpuNumber, bool enableIt)
{
    // Make sure the CPU number is valid.
    assert(cpuNumber <= 7);
    
    gicd_t * gicd = gic_get_gicd();
    uint8_t cpuMask = 1 << cpuNumber;
    
    // Like the priority registers, the target registers are byte accessible, and the register
    // struct has the them as a byte array, so we can just index directly by the
    // interrupt ID.
    if (enableIt)
    {
        gicd->ITARGETSRn[irqID] |= (cpuMask & 0xff);
    }
    else
    {
        gicd->ITARGETSRn[irqID] &= ~(cpuMask & 0xff);
    }
}

void gic_send_sgi(uint32_t irqID, uint32_t target_list, uint32_t filter_list)
{
    gicd_t * gicd = gic_get_gicd();
    
    gicd->SGIR = (filter_list << kBP_GICD_SGIR_TargetListFilter)
                    | (target_list << kBP_GICD_SGIR_CPUTargetList)
                    | (irqID & 0xf);
}

void gic_cpu_enable(bool enableIt)
{
    gicc_t * gicc = gic_get_gicc();
    
    if (enableIt)
    {
        gicc->CTLR |= kBM_GICC_CTLR_EnableS | kBM_GICC_CTLR_EnableNS;
    }
    else
    {
        gicc->CTLR &= ~(kBM_GICC_CTLR_EnableS | kBM_GICC_CTLR_EnableNS);
    }
}

void gic_set_cpu_priority_mask(uint32_t priority)
{
    gicc_t * gicc = gic_get_gicc();
    gicc->PMR = priority & 0xff;    
}

uint32_t gic_read_irq_ack(void)
{
    gicc_t * gicc = gic_get_gicc();
    return gicc->IAR;
}

void gic_write_end_of_irq(uint32_t irqID)
{
    gicc_t * gicc = gic_get_gicc();
    gicc->EOIR = irqID;
}

void gic_init(void)
{
    gicd_t * gicd = gic_get_gicd();

    // First disable the distributor.
    gic_enable(false);
    

    int i;
    for (i = 0; i < 32; ++i)
    {
    	gicd->ICENABLERn[i] = 0xFFFFFFFF;  // Disable all interrupts.
        gicd->ICPENDRn[i] = 0xFFFFFFFF;    // Clear all pending interrupts.
    }
    
    // Set all interrupts to secure.
    for (i = 0; i < 8; ++i)
    {
        gicd->IGROUPRn[i] = 0;
    }

    // Init the GIC CPU interface.
    gic_init_cpu();
    
    // Now enable the distributor.
    gic_enable(true);
}

void gic_init_cpu(void)
{
	 uint32_t i;
	  uint32_t irqRegs;
	  GIC_Type *gic = (GIC_Type *)GIC_REG_BASE;//(__get_CBAR() & 0xFFFF0000UL);

	  irqRegs = (gic->D_TYPER & 0x1FUL) + 1;

	  /* On POR, all SPI is in group 0, level-sensitive and using 1-N model */

	  /* Disable all PPI, SGI and SPI */
	  for (i = 0; i < irqRegs; i++)
	    gic->D_ICENABLER[i] = 0xFFFFFFFFUL;

	  /* Make all interrupts have higher priority */
	  gic->C_PMR = (0xFFUL << (8 - __GIC_PRIO_BITS)) & 0xFFUL;

	  /* No subpriority, all priority level allows preemption */
	  gic->C_BPR = 7 - __GIC_PRIO_BITS;

	  /* Enable group0 distribution */
	  gic->D_CTLR = 1UL;

	  /* Enable group0 signaling */
	  gic->C_CTLR = 1UL;

#if 0
    // Init the GIC CPU interface.
    gic_set_cpu_priority_mask(0xff);

    // Disable preemption.
    gicc_t * gicc = gic_get_gicc();
    gicc->BPR = 7;

    // Enable signaling the CPU.
    gic_cpu_enable(true);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
