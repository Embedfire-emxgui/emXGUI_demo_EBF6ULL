/*
 * imx_timer.h
 *
 *  Created on: 2016Äê3ÔÂ17ÈÕ
 *      Author: liuwei
 */

#ifndef SRC_BSP_IMX6_FIRMWARE_INC_IMX_TIMER_H_
#define SRC_BSP_IMX6_FIRMWARE_INC_IMX_TIMER_H_


#define regEPIT1_BASE SFR_PA2VA(0x020D0000) //!< Base address for EPIT instance number 1.
#define regEPIT2_BASE SFR_PA2VA(0x020D4000) //!< Base address for EPIT instance number 2.

#define EPIT_CR   0x00 // Control register
#define EPIT_SR   0x04 // Status register
#define EPIT_LR   0x08 // Load register
#define EPIT_CMPR 0x0C // Compare register
#define EPIT_CNR  0x10 // Counter register

typedef union
{
    u32 U;
    struct
    {
    	u32 EN    : 1; //!< [0] This bit enables the EPIT.
    	u32 ENMOD : 1; //!< [1] EPIT enable mode.
    	u32 OCIEN : 1; //!< [2] Output compare interrupt enable.
    	u32 RLD   : 1; //!< [3] Counter reload control.
    	u32 PRESCALAR : 12; //!< [15:4] Counter clock prescaler value.
    	u32 SWR    : 1; //!< [16] Software reset.
    	u32 IOVW   : 1; //!< [17] EPIT counter overwrite enable.
    	u32 DBGEN  : 1; //!< [18] This bit is used to keep the EPIT functional in debug mode.
    	u32 WAITEN : 1; //!< [19] This read/write control bit enables the operation of the EPIT during wait mode.
    	u32 RESERVED0 : 1; //!< [20] Reserved.
    	u32 STOPEN    : 1; //!< [21] EPIT stop mode enable.
    	u32 OM        : 2; //!< [23:22] EPIT output mode.This bit field determines the mode of EPIT output on the output pin.
    	u32 CLKSRC    : 2; //!< [25:24] Select clock source
    	u32 RESERVED1 : 6; //!< [31:26] Reserved.
    } B;
} EPIT_CR_t;
#define rEPIT_CR(base)   (*(volatile EPIT_CR_t *)(base+EPIT_CR))

typedef union
{
    u32 U;
    struct
    {
    	u32 OCIF : 1;  //!< [0] Output compare interrupt flag.
    	u32 RSV0 : 31; //!< [31:1] Reserved.
    } B;
} EPIT_SR_t;
#define rEPIT_SR(base)   (*(volatile EPIT_SR_t *)(base+EPIT_SR))

typedef union
{
	u32 U;
    struct
    {
		u32 LOAD : 32; //!< [31:0] Load value.
    } B;
} EPIT_LR_t;
#define rEPIT_LR(base)   (*(volatile EPIT_LR_t *)(base+EPIT_LR))

typedef union
{
    u32 U;
    struct
    {
    	u32 COMPARE : 32; //!< [31:0] Compare Value.
    } B;
} EPIT_CMPR_t;
#define rEPIT_CMPR(base)   (*(volatile EPIT_CMPR_t *)(base+EPIT_CMPR))

typedef union
{
	u32 U;
    struct
    {
		u32 COUNT : 32; //!< [31:0] Counter value.
    } B;
} EPIT_CNR_t;
#define rEPIT_CNR(base)   (*(volatile EPIT_CNR_t *)(base+EPIT_CNR))


#define	HW_EPIT1	(0)
#define	HW_EPIT2	(1)

//! @brief Options for low power mode support for the timers.
//!
//! These constants are bit masks that are or'd together to select in which low
//! power modes the timer will continue counting.
enum _timer_low_power_modes
{
    WAIT_MODE_EN = 1,   //!< Timer is enabled in wait mode.
    STOP_MODE_EN = 2    //!< Timer is enabled in stop mode.
};

//! @brief Available clock sources for the timers.
enum _timer_clock_sources
{
    CLKSRC_OFF = 0,     //!< clock source is OFF
	CLKSRC_PER_CLK = 1, //!< clock source is peripheral clock(IPG Clk)
    CLKSRC_CLKIN = 2,   //!< clock source is external from a CLKIN input
    CLKSRC_CKIL = 3     //!< clock source is low-freq reference clock
};

void epit_init(u32 epit_ch,u32 hz,bool reload_mode,  u32 low_power_mode);
void epit_setup_interrupt(u32 epit_ch, void (*irq_subroutine)(void), bool enableIt);

void epit_start(u32 epit_ch,bool irq_mode);
void epit_stop(u32 epit_ch);
void epit_irq_clean(u32 epit_ch);



#endif /* SRC_BSP_IMX6_FIRMWARE_INC_IMX_TIMER_H_ */
