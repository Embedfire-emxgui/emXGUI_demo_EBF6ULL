
	.cpu    cortex-a7
	//.cpu    arm926ej-s
    .fpu    vfpv3
    .syntax unified

/*=============================================================================================*/

#define	USERMODE    0x10
#define	FIQMODE     0x11
#define	IRQMODE     0x12
#define	SVCMODE     0x13
#define	SYSMODE		0x1F
#define	ABORTMODE   0x17
#define	UNDEFMODE   0x1B

#define	MODEMASK    0x1F
#define	IRQMASK		0x80
#define	FIQMASK		0x40


#define	IRQ_SP		0xFFF04000

/*=============================================================================================*/

	.extern  OSTCBCur
	.extern  OSTCBNew

	.extern  mk_isr_enter
	.extern  mk_isr_exit
	.extern	CPU_IRQHandler
	.extern Hook_TaskSW
	//.extern Hook_MTT_Switch

	.global	HAL_SchedulerStartup
	.global	HAL_ContexSwitch
	.global	HAL_ContexSwitchInISR

	.section  .arch_os,"ax",%progbits
	.text
/*=============================================================================================*/


	.macro	PUSH_FPU_REGS	 RX

	.endm

	.macro	POP_FPU_REGS	 RX

	.endm

//void PUSH_FPU_Regs(void *ptr)
	.global	PUSH_FPU_Regs
PUSH_FPU_Regs:
	VSTMIA	R0!,{S16-S31}
	BX	LR

//void POP_FPU_Regs(void *ptr)
	.global	POP_FPU_Regs
POP_FPU_Regs:
	VLDMIA	R0!,{S16-S31}
	BX	LR

/*=============================================================================================*/
	.global	OS_IRQHandle
OS_IRQHandle:

	STMFD   SP!, {R1-R3}			//; IRQ模式下,R1-R3 作为临时使用，先入栈保存
//;----------------------------------------------------------------------------
//;   R1--SP
//;	  R2--PC
//;   R3--SPSR
//;------------------------------------------------------------------------
	MOV     R1, SP
	ADD     SP, SP, #12             //;Adjust IRQ stack pointer
	SUB     R2, LR, #4              //;Adjust PC for return address to task
	MRS     R3, SPSR				//;Copy SPSR (Task CPSR)

	MSR     CPSR_cxsf, #SVCMODE|IRQMASK|FIQMASK   //;切回到线程所在的SVC mode


	//; SAVE TASK''S CONTEXT ONTO OLD TASK''S STACK //保存线程上下文
	STMFD   SP!, {R2}				//; Push task''s PC
	STMFD   SP!, {R4-R12, LR}		//; Push task''s LR,R12-R4

	LDMFD   R1!, {R4-R6}			//; Load Task''s R1-R3 from IRQ stack
	STMFD   SP!, {R4-R6}			//; Push Task''s R1-R3 to SVC stack
	STMFD   SP!, {R0}			    //; Push Task''s R0 to SVC stack
	STMFD   SP!, {R3}				//; Push task''s CPSR


	LDR     R1,=OSTCBCur            //; Save SP,OSTCBCur->OSTCBStkPtr=SP;
	LDR     R1,[R1]
	STR     SP,[R1]

	//////
 	BL		mk_isr_enter
	MSR    	CPSR_c,#IRQMODE|IRQMASK|FIQMASK    //; Change to IRQ mode to use IRQ stack to handle interrupt

	LDR		SP,=IRQ_SP

	BL		CPU_IRQHandler

    MSR		CPSR_c,#SVCMODE|IRQMASK|FIQMASK   //; Change to SVC mode
    BL 		mk_isr_exit
 	//////
	DMB


    LDMFD   SP!,{R4}                //; POP the task''s CPSR
    MSR		SPSR_cxsf,R4
    LDMFD   SP!,{R0-R12,LR,PC}^	    //; POP new Task''s context

/*=============================================================================================*/

HAL_SchedulerStartup:


	MSR     CPSR_cxsf,#SVCMODE|IRQMASK|FIQMASK     //Switch to SVC mode with IRQ&FIQ disable
	BL 		Hook_TaskSW
	B		POP_New


HAL_ContexSwitch:

	//// 保存当前线程上下文
	STMFD	SP!, {LR}           //;PUSH PC
	STMFD	SP!, {R0-R12, LR}   //;PUSH R0-R12 LR
	MRS		R0,  CPSR           //;Push CPSR
	STMFD	SP!, {R0}


	//// 保存当前线程的SP
	LDR		R0, =OSTCBCur 		//OSTCBCur->stk_cur = SP
	LDR		R0, [R0]
	STR		SP, [R0]


HAL_ContexSwitchInISR:
	//;----------------------------------------------------------------------------------
	//; Call OSTaskSwHook();
	//;----------------------------------------------------------------------------------

	//// 切换到新线程空间
	/*
	LDR		R0, =OSTCBNew
	LDR		R0, [R0]
	LDR		R1, =OSTCBCur
	LDR		R1, [R1]
	*/
	BL 		Hook_TaskSW

	//;----------------------------------------------------------------------------------
	//; OSTCBCur = OSTCBNew;
	//;----------------------------------------------------------------------------------
	LDR		R0, =OSTCBNew
	LDR		R1, =OSTCBCur
	LDR		R0, [R0]
	STR		R0, [R1]

POP_New:
	DMB
	//;----------------------------------------------------------------------------------
	//; 		SP = OSTCBNew->stk_cur;
	//;----------------------------------------------------------------------------------
	LDR		R0, =OSTCBNew
	LDR		R0, [R0]
	LDR		SP, [R0]


	//;----------------------------------------------------------------------------------
	//; Restore New Task context
	//;----------------------------------------------------------------------------------

	LDMFD 	SP!, {R0}              //;POP CPSR
	MSR 	SPSR_cxsf, R0
	LDMFD 	SP!, {R0-R12, LR, PC}^


/*=============================================================================================*/
//CPU_SR	HAL_EnterCritical(void);
	.global	HAL_EnterCritical

HAL_EnterCritical:
//	DMB
	MRS 	R0,CPSR
	//ORR 	R1,R0,#IRQMASK|FIQMASK
	//MSR		CPSR_c,R1
	CPSID   I
	NOP
	BX		LR

/*=============================================================================================*/
//void		HAL_ExitCritical(CPU_SR sr);
	.global	HAL_ExitCritical
HAL_ExitCritical:
/*
	AND		R0,R0,#(IRQMASK|FIQMASK)

	MRS		R1,CPSR
	AND		R1,R1,#~(IRQMASK|FIQMASK)

	ORR		R0,R0,R1
*/
	MSR     CPSR_c, R0
//	DMB
//	ISB

	BX		LR


/*=============================================================================================*/


	.end
