
#include  <string.h>

#include  "Include.h"
#include  "uart_drv.h"

/*============================================================================================*/

typedef struct
{
	const char *name;
	const struct uart_ops *uart_ops;
}uart_hal_t;

typedef struct __uart_dev
{
	//char szName[16];
	const struct uart_ops *uart_ops;

	mutex_obj *lock;
	BOOL is_open;

}uart_dev_t;

/*
typedef struct
{
	struct __uart_dev_t *dev;
	com_cfg_t   cfg;

}uart_obj_t;
*/

/*============================================================================================*/

static	sys_err_t	uart_dev_create(dev_obj_t *dev,const void *creata_pdata)
{
	uart_dev_t *uart_dev;
	uart_hal_t *uart_hal;

	uart_hal =(uart_hal_t*)creata_pdata;

	uart_dev =kmalloc(sizeof(uart_dev_t));
	if(uart_dev==NULL)
	{
		return SYS_ERR;
	}

	//strncpy(uart_dev->szName,uart_hal->name,32);
	uart_dev->uart_ops  =uart_hal->uart_ops;

	uart_dev->lock      =SYS_mutex_create(NULL);
	uart_dev->is_open   =FALSE;

	dev->dev_obj_pdata   =uart_dev;

	return	SYS_NO_ERR;
}

/*============================================================================================*/

static	sys_err_t	uart_dev_release(dev_obj_t *dev)
{
	uart_dev_t *uart_dev=(uart_dev_t*)dev->dev_obj_pdata;

	uart_dev->uart_ops->close();
	SYS_mutex_destroy(uart_dev->lock);

	kfree(uart_dev);

	return	SYS_NO_ERR;
}

/*============================================================================================*/

static	sys_err_t	uart_dev_open(dev_open_t *open,dev_obj_t *dev)
{
	uart_dev_t  *uart_dev=(uart_dev_t*)dev->dev_obj_pdata;
	sys_err_t err=SYS_ERR;

	open->dev_open_pdata =uart_dev;

	if(SYS_mutex_lock(uart_dev->lock,1000)==SYS_NO_ERR)
	{
		if(uart_dev->is_open==FALSE)
		{
			uart_dev->is_open=TRUE;
			uart_dev->uart_ops->open(9600);

			err =SYS_NO_ERR;
		}

		SYS_mutex_unlock(uart_dev->lock);
	}

	return	err;
}

/*============================================================================================*/

static	sys_err_t	uart_dev_close(dev_open_t *open)
{
	uart_dev_t  *uart_dev =(uart_dev_t*)open->dev_open_pdata;

	if(SYS_mutex_lock(uart_dev->lock,1000)==SYS_NO_ERR)
	{
		if(uart_dev->is_open==TRUE)
		{
			uart_dev->is_open	=FALSE;
		}

		SYS_mutex_unlock(uart_dev->lock);
	}

	return	SYS_NO_ERR;

}

/*============================================================================================*/

static	sys_err_t	uart_dev_ioctl(dev_open_t *open,int cmd,void *argv,int size)
{
	uart_dev_t  *uart_dev =(uart_dev_t*)open->dev_open_pdata;

	switch(cmd)
	{

		case	COM_IOCTL_SET_CFG:
				return	uart_dev->uart_ops->set_cfg((com_cfg_t*)argv);
				////////

		case	COM_IOCTL_GET_CFG:
				return	uart_dev->uart_ops->get_cfg((com_cfg_t*)argv);
				////////

		case	COM_IOCTL_WAIT_EVENT:
				return	SYS_NO_ERR;
				////////

		default:
				return SYS_NO_ERR;
	}
}

/*============================================================================================*/

static	sys_err_t	uart_dev_sync(dev_open_t *open)
{
	return	SYS_NO_ERR;
}

/*============================================================================================*/

static	s32	uart_dev_read(dev_open_t *open,void *buf,int size)
{
	uart_dev_t  *uart_dev =(uart_dev_t*)open->dev_open_pdata;

	return uart_dev->uart_ops->read(buf,size);
}

/*============================================================================================*/

static	s32	uart_dev_write(dev_open_t *open,const void *buf,int size)
{
	uart_dev_t  *uart_dev =(uart_dev_t*)open->dev_open_pdata;

	return uart_dev->uart_ops->write(buf,size);

}

/*============================================================================================*/

static	const	dev_ops_t dev_ops=
{

 uart_dev_create,
 uart_dev_release,
 uart_dev_open,
 uart_dev_close,
 uart_dev_ioctl,
 uart_dev_sync,
 uart_dev_write,
 uart_dev_read,
};


extern const struct uart_ops UART1_ops;
extern const struct uart_ops UART2_ops;
extern const struct uart_ops UART5_ops;

uart_hal_t UART1_hal={"UART1",&UART1_ops};
uart_hal_t UART2_hal={"UART2",&UART2_ops};
uart_hal_t UART5_hal={"UART5",&UART5_ops};


void	UART1_DevRegister(const char *dev_name)
{
	SYS_dev_create(dev_name,&dev_ops,&UART1_hal);
}

void	UART2_DevRegister(const char *dev_name)
{
	SYS_dev_create(dev_name,&dev_ops,&UART2_hal);
}

void	UART5_DevRegister(const char *dev_name)
{
	SYS_dev_create(dev_name,&dev_ops,&UART5_hal);
}


/*============================================================================================*/

#if 1

void UART_DevTest(void)
{
	char dat;
	int i,a,rx_cnt;
	file_obj *fp;
	com_cfg_t cfg;
	char buf[256];
	int n,delay;
	////

	UART1_DevRegister("COM1");

	fp	=SYS_open("DEV:COM1",O_RDWR,0);
	if(fp==NULL)
	{
		goto exit;
	}

	if(SYS_ioctl(fp,COM_IOCTL_GET_CFG,&cfg,sizeof(com_cfg_t)))
	{
		cfg.Baudrate=921600;
		cfg.Flags	= COM_DATA_BITS_8
					| COM_STOP_BITS_1
					| COM_PARITY_NONE
					| COM_FLOW_CONTROL_NONE
					| COM_INTERFACE_NORMAL;

		cfg.RxTimeOut =200;

		SYS_ioctl(fp,COM_IOCTL_SET_CFG,&cfg,sizeof(com_cfg_t));
	}

	n=x_sprintf(buf,"UART Dev Test.\r\n");
	SYS_write(fp,buf ,n);
	delay=0;

	while(1)
	{

		n =SYS_read(fp,buf,100);
    	if(n>0)
    	{
    		SYS_write(fp,buf,n);
    	}

    	if(delay>0)
    	{
    		SYS_msleep(delay);
    	}
	}


exit:

	SYS_close(fp);

}

#endif

/*============================================================================================*/

