#ifndef _SDMMC_SDCARD_H
#define _SDMMC_SDCARD_H

#include "BSP.h"
#include "fsl_card.h"

/*=========================================================================================*/

extern sd_card_t g_sd;
extern mmc_card_t g_mmc;

/*=========================================================================================*/

BOOL SDCard_DrvInit(void);
BOOL SDCard_drvReadBlock(u8 *buf,u32 blk_addr,int blk_count);
BOOL SDCard_drvWriteBlock(u8 *buf,u32 blk_addr,int blk_count);

/*=========================================================================================*/

#endif
