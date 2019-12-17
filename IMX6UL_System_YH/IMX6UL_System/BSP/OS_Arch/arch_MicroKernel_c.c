
#include <string.h>

#include "BSP.h"
#include "MicroKernel_Arch.h"

#define	FPU_EN	1

/*=========================================================================================*/
typedef struct _mk_thread mk_thread_t;

extern struct mk_thread_t *OSTCBCur;
extern struct mk_thread_t *OSTCBNew;

void*	HAL_StackInit(void *entry,void *exit, void *argv, void *stk_base, U32 stk_size)
{
    volatile u32 *stk;
    
	memset(stk_base,'C',stk_size);
   		
	stk  = (u32*)stk_base;
	stk += (stk_size>>2);
	stk	 = (u32*)((u32)stk&(~7));  //Align 8

    *(--stk) = (u32)entry;      /* Entry Point                                             */
    *(--stk) = (u32)exit;    	/* LR                                                      */
    *(--stk) = (u32)0;         	/* R12                                                     */
    *(--stk) = (u32)0;         	/* R11                                                     */
    *(--stk) = (u32)0;         	/* R10                                                     */
    *(--stk) = (u32)0;         	/* R9                                                      */
    *(--stk) = (u32)0;         	/* R8                                                      */
    *(--stk) = (u32)0;         	/* R7                                                      */
    *(--stk) = (u32)0;         	/* R6                                                      */
    *(--stk) = (u32)0;         	/* R5                                                      */
    *(--stk) = (u32)0;         	/* R4                                                      */
    *(--stk) = (u32)0;         	/* R3                                                      */
    *(--stk) = (u32)0;         	/* R2                                                      */
    *(--stk) = (u32)0;         	/* R1                                                      */
    *(--stk) = (u32)argv;		/* R0 : argument                                           */
    *(--stk) = (u32)SVCMODE;    /* CPSR  (SVC mode, Enable both IRQ and FIQ interrupts)    */

    return (void*)stk;
}

/*=============================================================================================================*/

void*		mk_thread_stack_base(mk_thread_t *ptcb);
u32			mk_thread_stack_size(mk_thread_t *ptcb);
void FreeRTOS_Tick_Handler( void );

void	HAL_StackInfo(u32 *pTot,u32 *pMaxUsed,u32 *pRem)
{
	u8 *p;
	s32 n,size;

	n=0;

//	size =mk_thread_stack_size(NULL);

//	p =(u8*)mk_thread_stack_base(NULL);

	while(*p=='C')
	{
		 n++;
		 p++;
	}

	if(pTot!=NULL)
	{
		*pTot =size;
	}

	if(pMaxUsed!=NULL)
	{
		*pMaxUsed =size-n;
	}

	if(pRem!=NULL)
	{
		*pRem =n;
	}
}

/*=============================================================================================================*/
static volatile float f1=1.3f,f2=36.6f,f3=325.0f,f4=0.0f;


#if 1

#define	TICK_HZ	200


static	void timer_isr(void)
{
#if 0
	f4 =f1*f2-f3;

	f1 += 13.317f;
	f2 -= 7.3f;
	f3 += 164.331f;
#endif

#if 0
	static int a=0;
	static int i=0;
	char buf[128];
	if(a++>TICK_HZ)
	{
		a=0;
		DebugPuts(".");
	}
#endif

	mk_tick_handler(TICK_HZ);
	Board_OnTick();
	epit_irq_clean(HW_EPIT1);
}

static void	TickTimer_Init(void)
{
//	DebugPuts("TickTimerInit.\r\n");
	epit_init(HW_EPIT1,TICK_HZ,TRUE,0);
	epit_setup_interrupt(HW_EPIT1,FreeRTOS_Tick_Handler,TRUE);
	epit_start(HW_EPIT1,TRUE);

}

#endif

/*=============================================================================================================*/

u32	HAL_TicksToTime(u32 ticks)
{
	u32 time;
 	
	time	=((ticks*1000U)/TICK_HZ);
 	return	time;	

}

u32	HAL_TimeToTicks(u32 time_ms)
{
	u32 ticks;
 	
 	ticks	=(time_ms*TICK_HZ)/1000U;
 	if(ticks<=0)	
 	{
 		ticks=1;
 	}
 	
 	return ticks;

}

/*=============================================================================================================*/
#if 0
void	Hook_ThreadSwitch(struct _mk_thread *new_tcb,struct _mk_thread *old_tcb)
{


}
#endif

void	Hook_ThreadDestroy(struct _mk_thread *tcb)
{
}

/*=============================================================================================================*/
void*	mk_thread_stack_base(struct _mk_thread *ptcb);
u32 	mk_thread_pid(struct _mk_thread *ptcb);
void	MTT_Switch(U32 new_pid,U32 old_pid);

#if FPU_EN
static u32 push_fpu_reg[16];
static u32 pop_fpu_reg[16];
void PUSH_FPU_Regs(void *ptr);
void POP_FPU_Regs(void *ptr);
#endif

//void Hook_MTT_Switch(struct _mk_thread *new_tcb,struct _mk_thread *old_tcb)
void Hook_TaskSW(void)
{

	u32 new_pid,old_pid;

#if	FPU_EN
	void *stk;

	stk =mk_thread_stack_base(OSTCBCur);
	PUSH_FPU_Regs(stk);
#endif

	new_pid =mk_thread_pid(OSTCBNew);
	old_pid =mk_thread_pid(OSTCBCur);
	MTT_Switch(new_pid,old_pid);

#if FPU_EN
	stk =mk_thread_stack_base(OSTCBNew);
	POP_FPU_Regs(stk);
#endif
}

/*=================================================================================================*/

void Hook_IsrEnter(u8 nest)
{

}

void Hook_IsrExit(void)
{

}

void Hook_IdleThreadStart(void)
{
	TickTimer_Init();
//	DebugPuts("Hook_IdleStart.\r\n");
}
 
void Hook_IdleThreadLoop(void)
{
	//WatchDog_Clr();
}

/*=================================================================================================*/

void CPU_IRQHandler(void)
{
	GIC_IRQHandler();
}

/*=============================================================================================================*/
