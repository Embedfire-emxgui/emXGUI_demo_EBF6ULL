/*
;--------------------------------------------
;
;名称:		IMX6UL 初始化配置文件
;修改时间:	2019-06-22
;作者:		Liuwei
;QQ:		304466616
;
;--------------------------------------------
*/

#include "cpu_def.h"
//#include "mx6.h"
//------------------------------------------------------------------------------

#define	INIT_SP		0x0093FFF0 //Onchip SRAM

#define	SP_INIT_UNDEF  	0xFFF00000
#define	SP_INIT_ABORT  	0xFFF01000
#define	SP_INIT_FIQ    	0xFFF02000
#define	SP_INIT_IRQ    	0xFFF04000
#define	SP_INIT_SYS    	0xFFF08000
#define	SP_INIT_SVC    	0xFFF0C000


///*===============================================================================*/


	.extern	Undef_Handler_C
	.extern	PaAbort_Handler_C
	.extern	DaAbort_Handler_C
	.extern	OS_IRQHandler
	.extern	CStartup


/*===============================================================================*/

	.cpu	cortex-a7
    .fpu    vfpv4//softvfp
    .syntax unified

    .section  .isr_vector,"ax",%progbits
	.global __start
	.global isr_vector

   	.global __vector_base
   	.global __vector_end


isr_vector:
__start:
__vector_base:
	b   ResetHandler
	ldr	pc, __undef		//handler for Undefined mode
	ldr	pc, __swi		//handler for SWI interrupt
	ldr	pc, __pa_abort	//handler for PAbort
	ldr	pc, __da_abort	//handler for DAbort
	.long	0x12345678	//reserved
	ldr	pc, __irq		//handler for IRQ interrupt
	ldr	pc, __fiq		//handler for FIQ interrupt

////////////////////////////////// For XBOOT
	.long	0xF00FA55A        //[8]    Tag0
	.long	0x00000000        //[9]    Rsv
	.long	__text_base       //[10]   __text_base
	.long	__text_end        //[11]   __text_end
	.long	__data_base       //[12]   __data_base
	.long	__data_end        //[13]   __data_end
	.long	__bss_base        //[14]   __bss_base
	.long	__bss_end         //[15]   __bss_end
////////////////////////////////


__reset:		.long	ResetHandler
__undef:	 	.long	Undef_Handler
__swi:	 		.long	SWI_Handler
__pa_abort:		.long	PaAbort_Handler
__da_abort:		.long	DaAbort_Handler
__irq:          .long   OS_IRQHandler
//__irq:          .long   IRQ_Handler
__fiq:	 		.long	0x00000000
__vector_end:	.long	0x00000000
				.byte   'I','M','X','6','U','L',0,0
				.byte   'S','y','s','t','e','m',0,0

////////////////////////////////////////

    .section  .text

IRQ_Handler:
		stmfd	sp!,{r0-r7,lr}

		bl		GIC_IRQHandler

		ldmfd	sp!,{r0-r7,lr}
		subs	pc,lr,#4

////////////////////////////////////////

		.global	Undef_Handler
Undef_Handler:
		stmfd	sp!,{r0-r7,lr}

		mov		r0,pc
		mov		r1,lr
		mrs		r2,cpsr
		mrs		r3,spsr
		bl		Undef_Handler_C
		ldmfd	sp!,{r0-r7,lr}

		add		lr,lr,#4 //?? return next addr

		subs	pc,lr,#0

;////////////////////////////////////////


SWI_Handle:

	//	stmfd	sp!,{r0-r3,lr}
	// bl	enter_kernel
	//	ldmfd	sp!,{r0-r3,lr}


	   	push	{r12,lr}
	   	and		r12,lr,#0xFF
/*

;		ldr		lr,	=swi_ret
;		ldr		r10,=sys_api_addr
;		add		r10,r7,LSL #2
;		ldr		pc,[r10]
*/

swi_ret:

	//	push	{r0-r3}
	//	bl		exit_kernel
	//	pop		{r0-r3}

		pop		{r12,lr}
		subs	pc,lr,#0

////////////////////////////////////////

		.global	PaAbort_Handler
PaAbort_Handler:
		stmfd	sp!,{r0-r7,lr}

		mov		r0,pc
		mov		r1,lr
		mrs		r2,cpsr
		mrs		r3,spsr
		bl		PaAbort_Handler_C
		ldmfd	sp!,{r0-r7,lr}

		add		lr,lr,#4 //?? return next addr

		subs	pc,lr,#4

////////////////////////////////////////

		.global	DaAbort_Handler
DaAbort_Handler:
		stmfd	sp!,{r0-r7,lr}

		mov		r0,pc
		mov		r1,lr
		mrs		r2,cpsr
		mrs		r3,spsr
		bl		DaAbort_Handler_C
		mov		r8,r0
		ldmfd	sp!,{r0-r7,lr}

		cmp		r8,#0
		addeq	lr,lr,#4 //?? return next addr

		subs	pc,lr,#8


////////////////////////////////////////

ResetHandler:
	nop
	nop
//	CPSIE   A
	nop
	nop
    mrs     r0,cpsr                 ;//取CPSR
    bic     r0,r0,#MODEMASK         ;//清模式位
    orr     r1,r0,#SVCMODE|NOINT    ;//设置为管理态，并禁止中断
    msr     cpsr_cxsf,r1            ;//切换到管理态,可防止意外返回0地址时出错.
	nop
	ldr		sp,=INIT_SP				;//设置临时栈指针，主内存等设置好后，将改在主内存


#if 1
	mov		r0, #0			// set up for MCR
	mcr		p15, 0, r0, c8, c7, 0	// invalidate TLBs
	mcr		p15, 0, r0, c7, c5, 0	// invalidate icache
	mcr		p15, 0, r0, c7, c5, 6	// invalidate BP array
	mcr     p15, 0, r0, c7, c10, 4	// DSB
	mcr     p15, 0, r0, c7, c5, 4	// ISB
	nop
	nop

	mrc 	p15,0,r0,c1,c0,0
	ldr		r1,=R1_M|R1_C|R1_W		// Disable MMU...
	bic		r0,r0,r1
	orr		r0,r0,#R1_I				// EnableICache
	mcr 	p15,0,r0,c1,c0,0
	nop
#endif


#if 1
    LDR     r0,=0xffffffff
    MCR     p15,0,r0,c1,c0,2        //Enable coprocessors
#endif

//	ldr		sp,=0x02027000

//	b		init //???
// 	BL  	inv_dcache

#if 1
    //;/* Enable NEON & VFP unit */
    mrc p15, 0, r1, c1, c0, 2
    orr r1, r1, #(0xf << 20)
    mcr p15, 0, r1, c1, c0, 2
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 4
    mov r0, #0x40000000
    fmxr fpexc, r0
#endif

#if 1
    ;/* Cache init */
    mrc p15, 0, r0, c0, c0, 0       ;/* read main ID register */
    and r1, r0, #0x00f00000         ;/* variant */
    and r2, r0, #0x0000000f         ;/* revision */
    orr r2, r2, r1, lsr #20-4       ;/* combine variant and revision */
    cmp r2, #0x30
    mrceq   p15, 0, r0, c1, c0, 1   ;/* read ACTLR */
    orreq   r0, r0, #0x6            ;/* Enable DP1(2), DP2(1) */
    mcreq   p15, 0, r0, c1, c0, 1   ;/* write ACTLR */

    ;/* Invalidate L1 I/D */
    mov r0, #0                      ;/* set up for MCR */
    mcr p15, 0, r0, c8, c7, 0       ;/* invalidate TLBs */
    mcr p15, 0, r0, c7, c5, 0       ;/* invalidate icache */

    ;/* Disable mmu stuff and caches */
//    mrc p15, 0, r0, c1, c0, 0
//    bic r0, r0, #0x00002000         ;/* clear bits 13 (--v-) */
//    bic r0, r0, #0x00000007         ;/* clear bits 2:0 (-cam) */
//    orr r0, r0, #0x00000002         ;/* set bit 1 (--a-) align */
//    orr r0, r0, #0x00000800         ;/* set bit 11 (z---) btb */
//    mcr p15, 0, r0, c1, c0, 0
#endif

	////
	mov		r0,pc
	bl		system_init_nommu

	;//MMU_Enable
    ldr     r0,=R1_M|R1_I|R1_C|R1_W|R1_Z|R1_V

   	mcr 	p15,0,r0,c1,c0,0
	nop
	nop
	isb
	nop
	ldr		pc,=virtual_start
	nop
	nop
virtual_start:
	nop
	nop
	nop
	nop

    ;///////////////////////////////////////////
	;//// 初始化栈空间.
InitStack:

	mov	r1,#UNDEFMODE|NOINT //UndefMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_UNDEF

	mov	r1,#ABORTMODE|NOINT //AbortMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_ABORT

	mov	r1,#FIQMODE|NOINT   //FIQMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_FIQ

 	mov	r1,#IRQMODE|NOINT   //IRQMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_IRQ

	mov	r1,#SYSMODE|NOINT   //SYSMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_SYS

	mov	r1,#SVCMODE|NOINT   //SVCMode
	msr	cpsr_cxsf,r1
    ldr sp,=SP_INIT_SVC
    nop
	nop


	mov		r0,#0
	mov		r1,#0
	mov		r2,#0
	mov		r3,#0
	ldr		pc,=CStartup
	nop
	nop
	b		.

  	;////////////////////////////////////////////////////

	.global	JumpToAddress
JumpToAddress:
	mov	pc,r0

  	;////////////////////////////////////////////////////

;/*===================================================================*/

   	.end

