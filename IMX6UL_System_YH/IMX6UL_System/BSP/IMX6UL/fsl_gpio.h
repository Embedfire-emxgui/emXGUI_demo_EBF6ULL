
#ifndef _FSL_GPIO_H_
#define _FSL_GPIO_H_

#include "fsl_common.h"

#define	GPIO1_REG_BASE	SFR_PA2VA(0x0209C000)
#define	GPIO2_REG_BASE	SFR_PA2VA(0x020A0000)
#define	GPIO3_REG_BASE	SFR_PA2VA(0x020A4000)
#define	GPIO4_REG_BASE	SFR_PA2VA(0x020A8000)
#define	GPIO5_REG_BASE	SFR_PA2VA(0x020AC000)

#if 0
#define	GPIO1	((GPIO_Type*)(GPIO1_REG_BASE))
#define	GPIO2	((GPIO_Type*)(GPIO2_REG_BASE))
#define	GPIO3	((GPIO_Type*)(GPIO3_REG_BASE))
#define	GPIO4	((GPIO_Type*)(GPIO4_REG_BASE))
#define	GPIO5	((GPIO_Type*)(GPIO5_REG_BASE))


/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct {
  __IO uint32_t DR;                                /**< GPIO data register, offset: 0x0 */
  __IO uint32_t GDIR;                              /**< GPIO direction register, offset: 0x4 */
  __I  uint32_t PSR;                               /**< GPIO pad status register, offset: 0x8 */
  __IO uint32_t ICR1;                              /**< GPIO interrupt configuration register1, offset: 0xC */
  __IO uint32_t ICR2;                              /**< GPIO interrupt configuration register2, offset: 0x10 */
  __IO uint32_t IMR;                               /**< GPIO interrupt mask register, offset: 0x14 */
  __IO uint32_t ISR;                               /**< GPIO interrupt status register, offset: 0x18 */
  __IO uint32_t EDGE_SEL;                          /**< GPIO edge select register, offset: 0x1C */
//       uint8_t RESERVED_0[100];
//  __O  uint32_t DR_SET;                            /**< GPIO data register SET, offset: 0x84 */
//  __O  uint32_t DR_CLEAR;                          /**< GPIO data register CLEAR, offset: 0x88 */
//  __O  uint32_t DR_TOGGLE;                         /**< GPIO data register TOGGLE, offset: 0x8C */
} GPIO_Type;

#endif

/*! @brief GPIO direction definition. */
typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput = 0U,  /*!< Set current pin as digital input.*/
    kGPIO_DigitalOutput = 1U, /*!< Set current pin as digital output.*/
} gpio_pin_direction_t;

/*! @brief GPIO interrupt mode definition. */
typedef enum _gpio_interrupt_mode
{
    kGPIO_NoIntmode = 0U,              /*!< Set current pin general IO functionality.*/
    kGPIO_IntLowLevel = 1U,            /*!< Set current pin interrupt is low-level sensitive.*/
    kGPIO_IntHighLevel = 2U,           /*!< Set current pin interrupt is high-level sensitive.*/
    kGPIO_IntRisingEdge = 3U,          /*!< Set current pin interrupt is rising-edge sensitive.*/
    kGPIO_IntFallingEdge = 4U,         /*!< Set current pin interrupt is falling-edge sensitive.*/
    kGPIO_IntRisingOrFallingEdge = 5U, /*!< Enable the edge select bit to override the ICR register's configuration.*/
} gpio_interrupt_mode_t;

/*! @brief GPIO Init structure definition. */
typedef struct _gpio_pin_config
{
    gpio_pin_direction_t direction; /*!< Specifies the pin direction. */
    uint8_t outputLogic;            /*!< Set a default output logic, which has no use in input */
    gpio_interrupt_mode_t interruptMode; /*!< Specifies the pin interrupt mode, a value of @ref gpio_interrupt_mode_t. */
} gpio_pin_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name GPIO Initialization and Configuration functions
 * @{
 */

/*!
 * @brief Initializes the GPIO peripheral according to the specified
 *        parameters in the initConfig.
 *
 * @param base GPIO base pointer.
 * @param pin Specifies the pin number
 * @param initConfig pointer to a @ref gpio_pin_config_t structure that
 *        contains the configuration information.
 */
void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *Config);
/*@}*/

/*!
 * @name GPIO Reads and Write Functions
 * @{
 */

/*!
 * @brief Sets the output level of the individual GPIO pin to logic 1 or 0.
 *
 * @param base GPIO base pointer.
 * @param pin GPIO port pin number.
 * @param output GPIOpin output logic level.
 *        - 0: corresponding pin output low-logic level.
 *        - 1: corresponding pin output high-logic level.
 */
void GPIO_PinWrite(GPIO_Type *base, uint32_t pin, uint8_t output);

/*!
 * @brief Sets the output level of the individual GPIO pin to logic 1 or 0.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PinWrite.
 */
static inline void GPIO_WritePinOutput(GPIO_Type *base, uint32_t pin, uint8_t output)
{
    GPIO_PinWrite(base, pin, output);
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 *
 * @param base GPIO peripheral base pointer (GPIO1, GPIO2, GPIO3, and so on.)
 * @param mask GPIO pin number macro
 */
static inline void GPIO_PortSet(GPIO_Type *base, uint32_t mask)
{
#if (defined(FSL_FEATURE_IGPIO_HAS_DR_SET) && (FSL_FEATURE_IGPIO_HAS_DR_SET == 1))
    base->DR_SET = mask;
#else
    base->DR |= mask;
#endif /* FSL_FEATURE_IGPIO_HAS_DR_SET */
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PortSet.
 */
static inline void GPIO_SetPinsOutput(GPIO_Type *base, uint32_t mask)
{
    GPIO_PortSet(base, mask);
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 0.
 *
 * @param base GPIO peripheral base pointer (GPIO1, GPIO2, GPIO3, and so on.)
 * @param mask GPIO pin number macro
 */
static inline void GPIO_PortClear(GPIO_Type *base, uint32_t mask)
{
#if (defined(FSL_FEATURE_IGPIO_HAS_DR_CLEAR) && (FSL_FEATURE_IGPIO_HAS_DR_CLEAR == 1))
    base->DR_CLEAR = mask;
#else
    base->DR &= ~mask;
#endif /* FSL_FEATURE_IGPIO_HAS_DR_CLEAR */
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 0.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PortClear.
 */
static inline void GPIO_ClearPinsOutput(GPIO_Type *base, uint32_t mask)
{
    GPIO_PortClear(base, mask);
}

/*!
 * @brief Reverses the current output logic of the multiple GPIO pins.
 *
 * @param base GPIO peripheral base pointer (GPIO1, GPIO2, GPIO3, and so on.)
 * @param mask GPIO pin number macro
 */
static inline void GPIO_PortToggle(GPIO_Type *base, uint32_t mask)
{
#if (defined(FSL_FEATURE_IGPIO_HAS_DR_TOGGLE) && (FSL_FEATURE_IGPIO_HAS_DR_TOGGLE == 1))
    base->DR_TOGGLE = mask;
#endif /* FSL_FEATURE_IGPIO_HAS_DR_TOGGLE */
}

/*!
 * @brief Reads the current input value of the GPIO port.
 *
 * @param base GPIO base pointer.
 * @param pin GPIO port pin number.
 * @retval GPIO port input value.
 */
static inline uint32_t GPIO_PinRead(GPIO_Type *base, uint32_t pin)
{
    assert(pin < 32);

    return (((base->DR) >> pin) & 0x1U);
}

/*!
 * @brief Reads the current input value of the GPIO port.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PinRead.
 */
static inline uint32_t GPIO_ReadPinInput(GPIO_Type *base, uint32_t pin)
{
    return GPIO_PinRead(base, pin);
}
/*@}*/

/*!
 * @name GPIO Reads Pad Status Functions
 * @{
 */

/*!
* @brief Reads the current GPIO pin pad status.
*
* @param base GPIO base pointer.
* @param pin GPIO port pin number.
* @retval GPIO pin pad status value.
*/
static inline uint8_t GPIO_PinReadPadStatus(GPIO_Type *base, uint32_t pin)
{
    assert(pin < 32);

    return (uint8_t)(((base->PSR) >> pin) & 0x1U);
}

/*!
* @brief Reads the current GPIO pin pad status.
* @deprecated Do not use this function.  It has been superceded by @ref GPIO_PinReadPadStatus.
*/
static inline uint8_t GPIO_ReadPadStatus(GPIO_Type *base, uint32_t pin)
{
    return GPIO_PinReadPadStatus(base, pin);
}

/*@}*/

/*!
 * @name Interrupts and flags management functions
 * @{
 */

/*!
 * @brief Sets the current pin interrupt mode.
 *
 * @param base GPIO base pointer.
 * @param pin GPIO port pin number.
 * @param pininterruptMode pointer to a @ref gpio_interrupt_mode_t structure
 *        that contains the interrupt mode information.
 */
void GPIO_PinSetInterruptConfig(GPIO_Type *base, uint32_t pin, gpio_interrupt_mode_t pinInterruptMode);

/*!
 * @brief Sets the current pin interrupt mode.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PinSetInterruptConfig.
 */
static inline void GPIO_SetPinInterruptConfig(GPIO_Type *base, uint32_t pin, gpio_interrupt_mode_t pinInterruptMode)
{
    GPIO_PinSetInterruptConfig(base, pin, pinInterruptMode);
}

/*!
 * @brief Enables the specific pin interrupt.
 *
 * @param base GPIO base pointer.
 * @param mask GPIO pin number macro.
 */
static inline void GPIO_PortEnableInterrupts(GPIO_Type *base, uint32_t mask)
{
    base->IMR |= mask;
}

/*!
 * @brief Enables the specific pin interrupt.
 *
 * @param base GPIO base pointer.
 * @param mask GPIO pin number macro.
 */
static inline void GPIO_EnableInterrupts(GPIO_Type *base, uint32_t mask)
{
    GPIO_PortEnableInterrupts(base, mask);
}

/*!
 * @brief Disables the specific pin interrupt.
 *
 * @param base GPIO base pointer.
 * @param mask GPIO pin number macro.
 */
static inline void GPIO_PortDisableInterrupts(GPIO_Type *base, uint32_t mask)
{
    base->IMR &= ~mask;
}

/*!
 * @brief Disables the specific pin interrupt.
 * @deprecated Do not use this function.  It has been superceded by @ref GPIO_PortDisableInterrupts.
 */
static inline void GPIO_DisableInterrupts(GPIO_Type *base, uint32_t mask)
{
    GPIO_PortDisableInterrupts(base, mask);
}

/*!
 * @brief Reads individual pin interrupt status.
 *
 * @param base GPIO base pointer.
 * @retval current pin interrupt status flag.
 */
static inline uint32_t GPIO_PortGetInterruptFlags(GPIO_Type *base)
{
    return base->ISR;
}

/*!
 * @brief Reads individual pin interrupt status.
 *
 * @param base GPIO base pointer.
 * @retval current pin interrupt status flag.
 */
static inline uint32_t GPIO_GetPinsInterruptFlags(GPIO_Type *base)
{
    return GPIO_PortGetInterruptFlags(base);
}

/*!
 * @brief Clears pin interrupt flag. Status flags are cleared by
 *        writing a 1 to the corresponding bit position.
 *
 * @param base GPIO base pointer.
 * @param mask GPIO pin number macro.
 */
static inline void GPIO_PortClearInterruptFlags(GPIO_Type *base, uint32_t mask)
{
    base->ISR = mask;
}

/*!
 * @brief Clears pin interrupt flag. Status flags are cleared by
 *        writing a 1 to the corresponding bit position.
 *
 * @param base GPIO base pointer.
 * @param mask GPIO pin number macro.
 */
static inline void GPIO_ClearPinsInterruptFlags(GPIO_Type *base, uint32_t mask)
{
    GPIO_PortClearInterruptFlags(base, mask);
}
/*@}*/

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */

#endif /* _FSL_GPIO_H_*/
