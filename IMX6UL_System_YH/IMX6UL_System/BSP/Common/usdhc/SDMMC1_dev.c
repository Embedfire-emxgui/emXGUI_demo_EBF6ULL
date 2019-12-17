
#include "bsp_sdcard.h"
#include "Include.h"

/*=========================================================================================*/

/*=========================================================================================*/

static	mutex_obj *mutex_lock=NULL;
static	BOOL card_is_rdy=FALSE;

/*=========================================================================================*/

static	sys_err_t	sd_disk_lock(void)
{
 	return	SYS_mutex_lock(mutex_lock,0xFFFFFFFF);
}

static	sys_err_t	sd_disk_unlock(void)
{
 	return	SYS_mutex_unlock(mutex_lock);
}


/*===============================================================================================*/

static	BOOL sd_disk_create(const void *pdata)
{
	mutex_lock =SYS_mutex_create(NULL);

	if(mutex_lock != NULL)
	{
		if(SDCard_DrvInit())
		{
			card_is_rdy	=TRUE;
			return	TRUE;
		}
	}

	card_is_rdy	=FALSE;
	return	FALSE;

}

static	BOOL sd_disk_delete(void)
{
	return TRUE;
}

/*=========================================================================================*/

static	BOOL sd_disk_open(void)
{
	if(card_is_rdy)
	{
		return	TRUE;
	}

	return TRUE;
}

/*=========================================================================================*/
//关闭SD Disk

static	BOOL	sd_disk_close(void)
{
	return TRUE;
}

/*=========================================================================================*/
//读SD Disk扇区

static BOOL sd_disk_read(U32 blk_addr,void *buf,U32 blk_count)
{
	int res=FALSE;

#if 1
	if(card_is_rdy)
	{

		sd_disk_lock();
		res=SDCard_drvReadBlock((u8*)buf,blk_addr,blk_count);
		sd_disk_unlock();
	}
#endif

	return res;
}

/*=========================================================================================*/
//写SD Disk扇区

static BOOL	sd_disk_write(U32 blk_addr,const void *buf,U32 blk_count)
{
	int res=FALSE;

#if 1
	if(card_is_rdy)
	{
		sd_disk_lock();
		res=SDCard_drvWriteBlock((u8*)buf,blk_addr,blk_count);
		sd_disk_unlock();
	}
#endif
	return res;
}


/*=========================================================================================*/

static	BOOL sd_disk_io_ctrl(int cmd,void *argv,int size)
{
	if(card_is_rdy==FALSE)
	{
		return FALSE;
	}

	switch(cmd)
	{
		case	BLK_DEV_GET_INFORMATION:

				if(argv)
				{
					blk_dev_info_t *pInfo =(blk_dev_info_t*)argv;

					pInfo->type         =BLK_DEV_TYPE_REMOVABLE;
					pInfo->sector_size  =512;
					pInfo->sector_count	=g_sd.blockCount;
					pInfo->block_size	=512;
					return	TRUE;

				}
				break;
				////////

		case	BLK_DEV_GET_STATUS:

				if(card_is_rdy)
				{
					*(int*)argv	=BLK_DEV_STATUS_OK;
				}
				else
				{
					*(int*)argv	=BLK_DEV_STATUS_REMOVED;
				}
				return	TRUE;
				////////


		default:
				break;

	}

	return FALSE;
}


/*=========================================================================================*/

//SD Disk数据结构

static const	blk_dev_ops_t blk_dev_ops={

sd_disk_lock,
sd_disk_unlock,
sd_disk_create,
sd_disk_delete,
sd_disk_open,
sd_disk_close,
sd_disk_read,
sd_disk_write,
sd_disk_io_ctrl,

};

/*=========================================================================================*/

#pragma pack(4)
static u8 buf[512];
#pragma pack()

static void sd_test(void)
{
	int res;
	int sec=0;

	while(sec < 0x00FFF0)
	{
		memset(buf,0,512);
		res=sd_disk_read(sec,(u8*)buf,1);
		if(!res)
		{
			DebugPrintf("sd_read err: %d\r\n",sec);
			SYS_msleep(1000);
		}
		else
		{
			DebugPutHex(sec*512,512,buf);
		}
		sec++;
		SYS_msleep(100);

	}
}


BOOL	SDMMC1_Register(const char *name)
{
	blk_dev_register(name,&blk_dev_ops,NULL);
//	sd_test();
	return TRUE;
}
