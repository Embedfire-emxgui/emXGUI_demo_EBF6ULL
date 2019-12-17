
#ifndef _FSL_PIT_H_
#define _FSL_PIT_H_

#include "fsl_common.h"

#define PIT_BASE                                 (0x40084000u)

#define PIT                                      ((PIT_Type *)PIT_BASE)

/** Array initializer of PIT peripheral base addresses */
#define PIT_BASE_ADDRS                           { PIT_BASE }
/** Array initializer of PIT peripheral base pointers */
#define PIT_BASE_PTRS                            { PIT }
/** Interrupt vectors for the PIT peripheral type */
#define PIT_IRQS                                 { { PIT_IRQn, PIT_IRQn, PIT_IRQn, PIT_IRQn } }

/* ----------------------------------------------------------------------------
   -- PIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Peripheral_Access_Layer PIT Peripheral Access Layer
 * @{
 */

/** PIT - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< PIT Module Control Register, offset: 0x0 */
       uint8_t RESERVED_0[220];
  __I  uint32_t LTMR64H;                           /**< PIT Upper Lifetime Timer Register, offset: 0xE0 */
  __I  uint32_t LTMR64L;                           /**< PIT Lower Lifetime Timer Register, offset: 0xE4 */
       uint8_t RESERVED_1[24];
  struct {                                         /* offset: 0x100, array step: 0x10 */
    __IO uint32_t LDVAL;                             /**< Timer Load Value Register, array offset: 0x100, array step: 0x10 */
    __I  uint32_t CVAL;                              /**< Current Timer Value Register, array offset: 0x104, array step: 0x10 */
    __IO uint32_t TCTRL;                             /**< Timer Control Register, array offset: 0x108, array step: 0x10 */
    __IO uint32_t TFLG;                              /**< Timer Flag Register, array offset: 0x10C, array step: 0x10 */
  } CHANNEL[4];
} PIT_Type;

/* ----------------------------------------------------------------------------
   -- PIT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PIT_Register_Masks PIT Register Masks
 * @{
 */

/*! @name MCR - PIT Module Control Register */
/*! @{ */
#define PIT_MCR_FRZ_MASK                         (0x1U)
#define PIT_MCR_FRZ_SHIFT                        (0U)
#define PIT_MCR_FRZ(x)                           (((uint32_t)(((uint32_t)(x)) << PIT_MCR_FRZ_SHIFT)) & PIT_MCR_FRZ_MASK)
#define PIT_MCR_MDIS_MASK                        (0x2U)
#define PIT_MCR_MDIS_SHIFT                       (1U)
#define PIT_MCR_MDIS(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_MCR_MDIS_SHIFT)) & PIT_MCR_MDIS_MASK)
/*! @} */

/*! @name LTMR64H - PIT Upper Lifetime Timer Register */
/*! @{ */
#define PIT_LTMR64H_LTH_MASK                     (0xFFFFFFFFU)
#define PIT_LTMR64H_LTH_SHIFT                    (0U)
#define PIT_LTMR64H_LTH(x)                       (((uint32_t)(((uint32_t)(x)) << PIT_LTMR64H_LTH_SHIFT)) & PIT_LTMR64H_LTH_MASK)
/*! @} */

/*! @name LTMR64L - PIT Lower Lifetime Timer Register */
/*! @{ */
#define PIT_LTMR64L_LTL_MASK                     (0xFFFFFFFFU)
#define PIT_LTMR64L_LTL_SHIFT                    (0U)
#define PIT_LTMR64L_LTL(x)                       (((uint32_t)(((uint32_t)(x)) << PIT_LTMR64L_LTL_SHIFT)) & PIT_LTMR64L_LTL_MASK)
/*! @} */

/*! @name LDVAL - Timer Load Value Register */
/*! @{ */
#define PIT_LDVAL_TSV_MASK                       (0xFFFFFFFFU)
#define PIT_LDVAL_TSV_SHIFT                      (0U)
#define PIT_LDVAL_TSV(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_LDVAL_TSV_SHIFT)) & PIT_LDVAL_TSV_MASK)
/*! @} */

/* The count of PIT_LDVAL */
#define PIT_LDVAL_COUNT                          (4U)

/*! @name CVAL - Current Timer Value Register */
/*! @{ */
#define PIT_CVAL_TVL_MASK                        (0xFFFFFFFFU)
#define PIT_CVAL_TVL_SHIFT                       (0U)
#define PIT_CVAL_TVL(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_CVAL_TVL_SHIFT)) & PIT_CVAL_TVL_MASK)
/*! @} */

/* The count of PIT_CVAL */
#define PIT_CVAL_COUNT                           (4U)

/*! @name TCTRL - Timer Control Register */
/*! @{ */
#define PIT_TCTRL_TEN_MASK                       (0x1U)
#define PIT_TCTRL_TEN_SHIFT                      (0U)
#define PIT_TCTRL_TEN(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_TEN_SHIFT)) & PIT_TCTRL_TEN_MASK)
#define PIT_TCTRL_TIE_MASK                       (0x2U)
#define PIT_TCTRL_TIE_SHIFT                      (1U)
#define PIT_TCTRL_TIE(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_TIE_SHIFT)) & PIT_TCTRL_TIE_MASK)
#define PIT_TCTRL_CHN_MASK                       (0x4U)
#define PIT_TCTRL_CHN_SHIFT                      (2U)
#define PIT_TCTRL_CHN(x)                         (((uint32_t)(((uint32_t)(x)) << PIT_TCTRL_CHN_SHIFT)) & PIT_TCTRL_CHN_MASK)
/*! @} */

/* The count of PIT_TCTRL */
#define PIT_TCTRL_COUNT                          (4U)

/*! @name TFLG - Timer Flag Register */
/*! @{ */
#define PIT_TFLG_TIF_MASK                        (0x1U)
#define PIT_TFLG_TIF_SHIFT                       (0U)
#define PIT_TFLG_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << PIT_TFLG_TIF_SHIFT)) & PIT_TFLG_TIF_MASK)
/*! @} */

/* The count of PIT_TFLG */
#define PIT_TFLG_COUNT                           (4U)



/*!
 * @brief List of PIT channels
 * @note Actual number of available channels is SoC dependent
 */
typedef enum _pit_chnl
{
    kPIT_Chnl_0 = 0U, /*!< PIT channel number 0*/
    kPIT_Chnl_1,      /*!< PIT channel number 1 */
    kPIT_Chnl_2,      /*!< PIT channel number 2 */
    kPIT_Chnl_3,      /*!< PIT channel number 3 */
} pit_chnl_t;

/*! @brief List of PIT interrupts */
typedef enum _pit_interrupt_enable
{
    kPIT_TimerInterruptEnable = PIT_TCTRL_TIE_MASK, /*!< Timer interrupt enable*/
} pit_interrupt_enable_t;

/*! @brief List of PIT status flags */
typedef enum _pit_status_flags
{
    kPIT_TimerFlag = PIT_TFLG_TIF_MASK, /*!< Timer flag */
} pit_status_flags_t;

/*!
 * @brief PIT configuration structure
 *
 * This structure holds the configuration settings for the PIT peripheral. To initialize this
 * structure to reasonable defaults, call the PIT_GetDefaultConfig() function and pass a
 * pointer to your config structure instance.
 *
 * The configuration structure can be made constant so it resides in flash.
 */
typedef struct _pit_config
{
    bool enableRunInDebug; /*!< true: Timers run in debug mode; false: Timers stop in debug mode */
} pit_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Ungates the PIT clock, enables the PIT module, and configures the peripheral for basic operations.
 *
 * @note This API should be called at the beginning of the application using the PIT driver.
 *
 * @param base   PIT peripheral base address
 * @param config Pointer to the user's PIT config structure
 */
void PIT_Init(PIT_Type *base, const pit_config_t *config);

/*!
 * @brief Gates the PIT clock and disables the PIT module.
 *
 * @param base PIT peripheral base address
 */
void PIT_Deinit(PIT_Type *base);

/*!
 * @brief Fills in the PIT configuration structure with the default settings.
 *
 * The default values are as follows.
 * @code
 *     config->enableRunInDebug = false;
 * @endcode
 * @param config Pointer to the onfiguration structure.
 */
static inline void PIT_GetDefaultConfig(pit_config_t *config)
{
    assert(config);

    /* Timers are stopped in Debug mode */
    config->enableRunInDebug = false;
}

#if defined(FSL_FEATURE_PIT_HAS_CHAIN_MODE) && FSL_FEATURE_PIT_HAS_CHAIN_MODE

/*!
 * @brief Enables or disables chaining a timer with the previous timer.
 *
 * When a timer has a chain mode enabled, it only counts after the previous
 * timer has expired. If the timer n-1 has counted down to 0, counter n
 * decrements the value by one. Each timer is 32-bits, which allows the developers
 * to chain timers together and form a longer timer (64-bits and larger). The first timer
 * (timer 0) can't be chained to any other timer.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number which is chained with the previous timer
 * @param enable  Enable or disable chain.
 *                true:  Current timer is chained with the previous timer.
 *                false: Timer doesn't chain with other timers.
 */
static inline void PIT_SetTimerChainMode(PIT_Type *base, pit_chnl_t channel, bool enable)
{
    if (enable)
    {
        base->CHANNEL[channel].TCTRL |= PIT_TCTRL_CHN_MASK;
    }
    else
    {
        base->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_CHN_MASK;
    }
}

#endif /* FSL_FEATURE_PIT_HAS_CHAIN_MODE */

/*! @}*/

/*!
 * @name Interrupt Interface
 * @{
 */

/*!
 * @brief Enables the selected PIT interrupts.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 * @param mask    The interrupts to enable. This is a logical OR of members of the
 *                enumeration ::pit_interrupt_enable_t
 */
static inline void PIT_EnableInterrupts(PIT_Type *base, pit_chnl_t channel, uint32_t mask)
{
    base->CHANNEL[channel].TCTRL |= mask;
}

/*!
 * @brief Disables the selected PIT interrupts.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 * @param mask    The interrupts to disable. This is a logical OR of members of the
 *                enumeration ::pit_interrupt_enable_t
 */
static inline void PIT_DisableInterrupts(PIT_Type *base, pit_chnl_t channel, uint32_t mask)
{
    base->CHANNEL[channel].TCTRL &= ~mask;
}

/*!
 * @brief Gets the enabled PIT interrupts.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::pit_interrupt_enable_t
 */
static inline uint32_t PIT_GetEnabledInterrupts(PIT_Type *base, pit_chnl_t channel)
{
    return (base->CHANNEL[channel].TCTRL & PIT_TCTRL_TIE_MASK);
}

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the PIT status flags.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::pit_status_flags_t
 */
static inline uint32_t PIT_GetStatusFlags(PIT_Type *base, pit_chnl_t channel)
{
    return (base->CHANNEL[channel].TFLG & PIT_TFLG_TIF_MASK);
}

/*!
 * @brief  Clears the PIT status flags.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 * @param mask    The status flags to clear. This is a logical OR of members of the
 *                enumeration ::pit_status_flags_t
 */
static inline void PIT_ClearStatusFlags(PIT_Type *base, pit_chnl_t channel, uint32_t mask)
{
    base->CHANNEL[channel].TFLG = mask;
}

/*! @}*/

/*!
 * @name Read and Write the timer period
 * @{
 */

/*!
 * @brief Sets the timer period in units of count.
 *
 * Timers begin counting from the value set by this function until it reaches 0,
 * then it generates an interrupt and load this register value again.
 * Writing a new value to this register does not restart the timer. Instead, the value
 * is loaded after the timer expires.
 *
 * @note Users can call the utility macros provided in fsl_common.h to convert to ticks.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 * @param count   Timer period in units of ticks
 */
static inline void PIT_SetTimerPeriod(PIT_Type *base, pit_chnl_t channel, uint32_t count)
{
    base->CHANNEL[channel].LDVAL = count;
}

/*!
 * @brief Reads the current timer counting value.
 *
 * This function returns the real-time timer counting value, in a range from 0 to a
 * timer period.
 *
 * @note Users can call the utility macros provided in fsl_common.h to convert ticks to usec or msec.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number
 *
 * @return Current timer counting value in ticks
 */
static inline uint32_t PIT_GetCurrentTimerCount(PIT_Type *base, pit_chnl_t channel)
{
    return base->CHANNEL[channel].CVAL;
}

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the timer counting.
 *
 * After calling this function, timers load period value, count down to 0 and
 * then load the respective start value again. Each time a timer reaches 0,
 * it generates a trigger pulse and sets the timeout interrupt flag.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number.
 */
static inline void PIT_StartTimer(PIT_Type *base, pit_chnl_t channel)
{
    base->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
}

/*!
 * @brief Stops the timer counting.
 *
 * This function stops every timer counting. Timers reload their periods
 * respectively after the next time they call the PIT_DRV_StartTimer.
 *
 * @param base    PIT peripheral base address
 * @param channel Timer channel number.
 */
static inline void PIT_StopTimer(PIT_Type *base, pit_chnl_t channel)
{
    base->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

/*! @}*/

#if defined(FSL_FEATURE_PIT_HAS_LIFETIME_TIMER) && FSL_FEATURE_PIT_HAS_LIFETIME_TIMER

/*!
 * @brief Reads the current lifetime counter value.
 *
 * The lifetime timer is a 64-bit timer which chains timer 0 and timer 1 together.
 * Timer 0 and 1 are chained by calling the PIT_SetTimerChainMode before using this timer.
 * The period of lifetime timer is equal to the "period of timer 0 * period of timer 1".
 * For the 64-bit value, the higher 32-bit has the value of timer 1, and the lower 32-bit
 * has the value of timer 0.
 *
 * @param base PIT peripheral base address
 *
 * @return Current lifetime timer value
 */
uint64_t PIT_GetLifetimeTimerCount(PIT_Type *base);

#endif /* FSL_FEATURE_PIT_HAS_LIFETIME_TIMER */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_PIT_H_ */
