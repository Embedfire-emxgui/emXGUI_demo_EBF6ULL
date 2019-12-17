								
;******************************************************************************************************** */

USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0

;*********************************************************************************************************
;                                    EXPORT and EXTERNAL REFERENCES
;*********************************************************************************************************/

	IMPORT  OSTCBCur
	IMPORT  OSTCBHighRdy
	
			
	IMPORT  mk_interrupt_enter
	IMPORT  mk_interrupt_exit
	;IMPORT  OSTaskSwHook
	
	IMPORT	CPU_IRQHandler
	

	EXPORT  HAL_EnterCritical
	EXPORT  HAL_ExitCritical
	EXPORT  HAL_SchedulerStartup
	EXPORT  HAL_ContexSwitch
	EXPORT  HAL_ContexSwitchInterruptMode
		
	EXPORT	OS_IRQHandler
	
	
	PRESERVE8
	CODE32	
	AREA OS_CPU_ASM, CODE, READONLY,ALIGN=3

;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; The stack frame is assumed to look as follows:
;
;							    Entry Point(Task Name)				(High memory)
;                               LR(R14)
;                               R12
;                               R11
;                               R10
;                               R9
;                               R8
;                               R7
;                               R6
;                               R5
;                               R4
;                               R3
;                               R2
;                               R1
;                               R0 : argument
; OSTCBHighRdy->OSTCBStkPtr --> CPSR								(Low memory)
;
; Note : OSStartSched() MUST:
;           a) Call OSTaskSwHook() then,
;           b) Switch to the highest priority task.
;********************************************************************************************************** */
;/*********************************************************************************************/
HAL_SchedulerStartup  
	
	MSR     CPSR_cxsf,#SVCMODE|NOINT     ;Switch to SVC mode with IRQ&FIQ disable
	
	;//BL		OSTaskSwHook            ;Call user define Task switch hook
	
	;----------------------------------------------------------------------------------		
	; 		SP = OSTCBHighRdy->OSTCBStkPtr;
	;----------------------------------------------------------------------------------	
	LDR 	R0, =OSTCBHighRdy
	LDR 	R0, [R0]         
	LDR 	SP, [R0]         

	;----------------------------------------------------------------------------------		
	; Prepare to return to proper mode
	;----------------------------------------------------------------------------------	
	LDMFD 	SP!, {R0}  
	MSR 	SPSR_cxsf, R0
	LDMFD 	SP!, {R0-R12, LR, PC}^


;**********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s): 	   1) Upon entry: 
;              	  OSTCBCur      points to the OS_TCB of the task to suspend
;              	  OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;          	   2) The stack frame of the task to suspend looks as follows:
;                                                   
;                                                   PC                    (High memory)
;				  									LR(R14)					
;           					                    R12
; 			                      			        R11
;           		                			    R10
;                   		           			 	R9
;                           		    			R8
;                               					R7
;                               					R6
;                               					R5
;                               					R4
;                               					R3
;                               					R2
;                               					R1
;                               					R0
; 						OSTCBCur->OSTCBStkPtr ----> CPSR					(Low memory)
;
;
;          	   3) The stack frame of the task to resume looks as follows:
;
;			  		  								PC				(High memory)
;                                                   LR(R14)	
;			           			                    R12
;           		            			        R11
;                   		        			    R10
;                           		   			 	R9
;                               					R8
;                               					R7
;			                               			R6
;           		                    			R5
;                   		            			R4
;                           		    			R3
;                               					R2
;                               					R1
;			                               			R0
; 					OSTCBHighRdy->OSTCBStkPtr ---->	CPSR					(Low memory)
;*********************************************************************************************************/
;/*********************************************************************************************/
HAL_ContexSwitch
	
	STMFD	SP!, {LR}           ;PC
	STMFD	SP!, {R0-R12, LR}   ;R0-R12 LR
	MRS		R0,  CPSR       ;Push CPSR
	STMFD	SP!, {R0}	
		
	;----------------------------------------------------------------------------------
	; 		OSTCBCur->OSTCBStkPtr = SP
	;----------------------------------------------------------------------------------		
	LDR		R0, =OSTCBCur
	LDR		R0, [R0]
	STR		SP, [R0]
	
	;----------------------------------------------------------------------------------		
	; OSTaskSwHook();
	;---------------------------------------------------------------------------------	
	;//BL 		OSTaskSwHook

	;----------------------------------------------------------------------------------			
	; OSTCBCur = OSTCBHighRdy;
	;----------------------------------------------------------------------------------		
	LDR		R0, =OSTCBHighRdy
	LDR		R1, =OSTCBCur
	LDR		R0, [R0]
	STR		R0, [R1]
	
	
	;----------------------------------------------------------------------------------		
	;  SP = OSTCBHighRdy->OSTCBStkPtr
	;----------------------------------------------------------------------------------		
	LDR		R0, =OSTCBHighRdy
	LDR		R0, [R0]
	LDR		SP, [R0]

	;----------------------------------------------------------------------------------	
	;Restore New task context
	;----------------------------------------------------------------------------------	
	LDMFD 	SP!, {R0}		;POP CPSR
	MSR 	SPSR_cxsf, R0
	LDMFD 	SP!, {R0-R12, LR, PC}^	

	
	
;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntrCtxSw(void)
;
; Description: 1) This code performs a context switch if a higher priority task has been made ready-to-run
;               	during an ISR.
;
;          	   2) The stack frame of the task to suspend looks as follows:
;
;				  									PC					(High memory)
;                                                   LR(R14)
;           					                    R12
; 			                      			        R11
;           		                			    R10
;                   		           			 	R9
;                           		    			R8
;                               					R7
;                               					R6
;                               					R5
;                               					R4
;                               					R3
;                               					R2
;                               					R1
;                               					R0
;                               					
; 						OSTCBCur->OSTCBStkPtr ----> CPSR					(Low memory)
;
;
;          	   3) The stack frame of the task to resume looks as follows:
;
;			  		  								PC					(High memory)
;                                                   LR(R14)	
;			           			                    R12
;           		            			        R11
;                   		        			    R10
;                           		   			 	R9
;                               					R8
;                               					R7
;			                               			R6
;           		                    			R5
;                   		            			R4
;                           		    			R3
;                               					R2
;                               					R1
;			                               			R0
; 					OSTCBHighRdy->OSTCBStkPtr ---->	CPSR					(Low memory)
;*********************************************************************************************************/
;/*********************************************************************************************/

HAL_ContexSwitchInterruptMode
	;----------------------------------------------------------------------------------		
	; Call OSTaskSwHook();
	;----------------------------------------------------------------------------------	
	;//BL 		OSTaskSwHook
	
	;----------------------------------------------------------------------------------			
	; OSTCBCur = OSTCBHighRdy;
	;----------------------------------------------------------------------------------		
	LDR		R0, =OSTCBHighRdy
	LDR		R1, =OSTCBCur
	LDR		R0, [R0]
	STR		R0, [R1]
		
	;----------------------------------------------------------------------------------		
	; 		SP = OSTCBHighRdy->OSTCBStkPtr;
	;----------------------------------------------------------------------------------		
	LDR		R0, =OSTCBHighRdy
	LDR		R0, [R0]
	LDR		SP, [R0]
	
	;----------------------------------------------------------------------------------	
	; Restore New Task context
	;----------------------------------------------------------------------------------	
	LDMFD 	SP!, {R0}              ;POP CPSR
	MSR 	SPSR_cxsf, R0
	LDMFD 	SP!, {R0-R12, LR, PC}^	
	

;/*********************************************************************************************/
	
OS_IRQHandler 	

	STMFD   SP!, {R1-R3}			; We will use R1-R3 as temporary registers
;----------------------------------------------------------------------------
;   R1--SP
;	R2--PC 
;   R3--SPSR
;------------------------------------------------------------------------
	MOV     R1, SP
	ADD     SP, SP, #12             ;Adjust IRQ stack pointer
	SUB     R2, LR, #4              ;Adjust PC for return address to task

	MRS     R3, SPSR				; Copy SPSR (Task CPSR)
	
   

	MSR     CPSR_cxsf, #SVCMODE|NOINT   ;Change to SVC mode

									; SAVE TASK''S CONTEXT ONTO OLD TASK''S STACK
									
	STMFD   SP!, {R2}				; Push task''s PC 
	STMFD   SP!, {R4-R12, LR}		; Push task''s LR,R12-R4
	
	LDMFD   R1!, {R4-R6}			; Load Task''s R1-R3 from IRQ stack 
	STMFD   SP!, {R4-R6}			; Push Task''s R1-R3 to SVC stack
	STMFD   SP!, {R0}			    ; Push Task''s R0 to SVC stack
	
	STMFD   SP!, {R3}				; Push task''s CPSR
		 
	LDR     R1,=OSTCBCur            ; Save SP,OSTCBHighRdy->OSTCBStkPtr=SP;
	LDR     R1,[R1]
	STR     SP,[R1]                 
	
 	BL		mk_interrupt_enter

	MSR    	CPSR_c,#IRQMODE|NOINT    ; Change to IRQ mode to use IRQ stack to handle interrupt
	
	BL		CPU_IRQHandler
    
    MSR		CPSR_c,#SVCMODE|NOINT   ; Change to SVC mode
  
    BL 		mk_interrupt_exit           	
    
    LDMFD   SP!,{R4}                ; POP the task''s CPSR 
    MSR		SPSR_cxsf,R4
    LDMFD   SP!,{R0-R12,LR,PC}^	    ; POP new Task''s context



;/*********************************************************************************************/
;//CPU_SR	HAL_EnterCritical(void);
 
HAL_EnterCritical
	
	MRS 	R0,CPSR
	ORR 	R1,R0,#0xC0
	MSR		CPSR_c,R1
	NOP
	BX		LR

;/*********************************************************************************************/
;//void		HAL_ExitCritical(CPU_SR sr);

HAL_ExitCritical

	AND		R0,R0,#0xC0
	
	MRS		R1,CPSR	
	AND		R1,R1,#~0xC0
		
	ORR		R0,R0,R1	
	MSR     CPSR_c, R0
	BX		LR
	
	
;/*********************************************************************************************/

		
		
		MACRO
		API_ENTRY	 $Label,$Id	
		EXPORT	$Label			
		AREA	$Label,CODE,READONLY,ALIGN=3
		CODE32
;$Label
		push	{lr}
		ldr		lr,%F1
		ldr		r7,%F2
		svc		0x10
1			
		pop		{pc}
2
		dcd		$Id
				
		MEND

	CODE32	
	AREA ooOS_CPU_ASM, CODE, READONLY,ALIGN=5

	API_ENTRY	__puts,		0x0040
	API_ENTRY	__sleep,	0x0041
	API_ENTRY	__sprintf,	0x0042
	API_ENTRY	__create_thread,	0x0130

;//	API_ENTRY	__puts2,128
	
	END
