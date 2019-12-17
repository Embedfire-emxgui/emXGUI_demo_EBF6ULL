

#ifndef		__INCLUDE_H__
#define		__INCLUDE_H__

#ifdef	__cplusplus
extern "C"{
#endif

//#include	<stddef.h>
//#include	<stdint.h>
//#include	<stdio.h>
#include	<string.h>
#include	<math.h>

/*=========================================================================================*/


#define	_T(s)	(WCHAR*)L##s

#include	"def.h"
#include	"Kernel.h"
#include	"emXGUI.h"
#include	"x_libc.h"
#include	"x_file.h"
#include	"X_SDK.h"

#include	"MicroKernel_Arch.h"
#include	"Kernel_Arch.h"
#include	"emXGUI_Arch.h"

#include	"BSP.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/*=========================================================================================*/



/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
