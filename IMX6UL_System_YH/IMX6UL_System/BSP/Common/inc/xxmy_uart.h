#ifndef __MY_UART_H__
#define __MY_UART_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"
#include "Kernel.h"
/*=========================================================================================*/
#if 0

struct	uart_ops
{

	BOOL (*Open)(u32 baud);
	void (*Close)(void);
	BOOL (*SetCfg)(com_cfg_t *cfg);
	BOOL (*GetCfg)(com_cfg_t *cfg);
	int  (*Write)(const u8 *buf,int count);
	int  (*Read)(u8 *buf,int count);

};
#endif
/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__MY_UART_H__*/














