/*
 * uart_drv.h
 *
 *  Created on: 2018Äê12ÔÂ20ÈÕ
 *      Author: liuwei
 */

#ifndef __UART_DRV_H__
#define __UART_DRV_H__

#include "Kernel.h"
#include "fsl_uart.h"

/*===================================================================================*/

struct	uart_ops
{

	BOOL (*open)(u32 baud);
	void (*close)(void);
	BOOL (*set_cfg)(com_cfg_t *cfg);
	BOOL (*get_cfg)(com_cfg_t *cfg);
	int  (*write)(const u8 *buf,int count);
	int  (*read)(u8 *buf,int count);

};


/*===================================================================================*/



#endif /* __UART_DRV_H__ */
