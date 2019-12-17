

#ifndef		__BSP_H__
#define		__BSP_H__

 
#ifdef	__cplusplus
extern "C"{
#endif



/*=========================================================================================*/

#define	KHZ	(1000)
#define	MHZ	(1000000)

/*=========================================================================================*/

#include	"def.h"

#include	"ARMv7/arch_arm.h"

#include	"MicroKernel_Arch.h"
#include	"Kernel_Arch.h"

//#include	"Board/ITOP_IMX6UL/Board.h"
#include	"Board/YH_IMX6ULL/Board.h"

/*=========================================================================================*/

extern	const	U8	__text_base[],__text_end[];
extern	const	U8	__data_base[],__data_end[];
extern	const	U8	__bss_base[],__bss_end[];


/*=========================================================================================*/
void CPP_Entry(void);

/*=========================================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif

