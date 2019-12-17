
#include 	"Include.h"

#if 0

/*=========================================================================================*/
/*=========================================================================================*/

#define	RAM_DISK_SECTOR_SIZE	512


static	char*	ram_disk_addr_base;

/*=========================================================================================*/

static	mutex_obj	*mutex_lock;

static	sys_err_t	ram_disk_lock(void)
{
 	return	SYS_mutex_lock(mutex_lock,0xFFFFFFFF);
}

static	sys_err_t	ram_disk_unlock(void)
{
 	return	SYS_mutex_unlock(mutex_lock);
}

/*=========================================================================================*/

//初始化RAM Disk
static	BOOL	ram_disk_create(const void *pdata)
{
	//ram_disk_addr_base	=PA2VA(RAM_DISK_START_ADDR);
	//DebugPrintf("ram_disk_addr_base =%08XH\r\n",ram_disk_addr_base);

	return	TRUE;
}

static	BOOL	ram_disk_delete(void)
{
	return TRUE;
}

/*=========================================================================================*/

static	BOOL	ram_disk_open(void)
{

	return TRUE;
}

/*=========================================================================================*/
//关闭RAM Disk


static	BOOL	ram_disk_close(void)
{
	
	return TRUE;
}


/*=========================================================================================*/
/*=========================================================================================*/
//读RAM Disk

static	BOOL ram_disk_read(U32  page_addr,void *buf,U32 page_count)
{
	//DebugPrintf("ram_disk_rd -> %08XH - %d\n",addr,count);
	memcpy(buf,(void*)((page_addr<<9)+ram_disk_addr_base),page_count<<9);
	return TRUE;
}

/*=========================================================================================*/
//写RAM Disk

static	BOOL	ram_disk_write(U32 page_addr,const void *buf,U32 page_count)
{
	//DebugPrintf("ram_disk_wt -> %08XH - %d\n",addr,count);
	memcpy((void*)((page_addr<<9)+ram_disk_addr_base),buf,page_count<<9);
	return TRUE;
}

/*=========================================================================================*/

static	BOOL	ram_disk_ioctl(int cmd,void *argv,int size)
{
	
	
	switch(cmd)
	{
		case	BLK_DEV_GET_INFORMATION:
				
				if(argv)
				{
					blk_dev_info_t *pInfo;
					pInfo=argv;
						
					pInfo->type					=BLK_DEV_TYPE_RAMDISK;
					pInfo->sector_size			=RAM_DISK_SECTOR_SIZE;
					pInfo->sector_count			=RAM_DISK_SIZE/RAM_DISK_SECTOR_SIZE;
					pInfo->block_size			=RAM_DISK_SECTOR_SIZE;
					return	TRUE;
				}
				break;
				////////
				
		case	BLK_DEV_GET_STATUS:
				
				*(int*)argv	=BLK_DEV_STATUS_OK;
				return	TRUE;
				////////

					
		default:
				break;
	
	}
	return	FALSE;
}
/*=========================================================================================*/
/*=========================================================================================*/

//RAM Disk数据结构

static const	blk_dev_ops_t blk_dev_ops={

ram_disk_lock,
ram_disk_unlock,
ram_disk_create,
ram_disk_delete,
ram_disk_open,
ram_disk_close,
ram_disk_read,
ram_disk_write,
ram_disk_ioctl,
};

/*=========================================================================================*/


void	RAMDisk_Register(const char *name)
{
	ram_disk_addr_base	=(char*)RAM_DISK_VA_BASE;

	mutex_lock=SYS_mutex_create(NULL);
	 
	blk_dev_register(name,&blk_dev_ops,NULL);
	
}

/*=========================================================================================*/
/*=========================================================================================*/
#endif
