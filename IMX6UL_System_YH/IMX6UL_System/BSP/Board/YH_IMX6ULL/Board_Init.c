#include	"Include.h"
#include	"BSP.h"

/*=========================================================================================*/

static int a=0x1234;

u32 UPLL_Clk,APLL_Clk,SYS_Clk,PCLK,CPU_Clk,HClk1,HClk234;

static void epit_isr(void)
{
	static volatile int cnt=0;
	cnt++;
	if(cnt&1)
	{
		LED_ON();
	}
	else
	{
		LED_OFF();
	}
	epit_irq_clean(HW_EPIT1);
}
void Board_HardInit(void)
{
	char buf[128];
	u32 a;

	Board_LowLevelInit();


//	ARM_EnableIRQ();
//	while(1);
//	led_test(10);
}

/*=========================================================================================*/
void	vfs_fatfs_init(void);
void	vfs_fatfs_register(int drv,const char *fs_name,const char *blk_dev_name);
void	vfs_fatfs_check(void);


static  void	__FileSystemInit(void)
{
#if 1

	////BlockDevice
	//RAMDisk_Register("RAMDisk");
	//NandDisk_Register("NandFlash");
	SDMMC1_Register("SDMMC1");
	//SDMMC2_Register("SDMMC2");


	////FileSystem
	vfs_fatfs_init();

	//vfs_fatfs_register(0,"A:","RAMDisk");
	vfs_fatfs_register(1,"B:","SDMMC1");
	//vfs_fatfs_register(2,"C:","NandFlash");

	vfs_fatfs_check();
	////////
#endif
}

/*=========================================================================================*/


static  void	__DriversInit(void)
{

	PortInit_HC595();
	I2C_P1_28_P1_29.Init();
	I2C_P1_30_P1_31.Init();

	RX8010_Init(&I2C_P1_30_P1_31);


//	I2C_PG0_PG1.Init();
//	I2C_PG2_PG3.Init();

//	NAU8822_Init(&I2C_PG0_PG1);
//	ISL1208_Init(&I2C_PG0_PG1);

//	OEM_Net_Init();

//	UART1_DevRegister("COM1");
//	UART2_DevRegister("COM2");

//	ALCO_Register();
//	SPI_Flash_Register();

	Audio_DevInit();

}

/*=========================================================================================*/


void	ProgressModuleInit(void);       //进程模块初始化

void	Board_ModuleInit(void)
{
	////系统模块初始化
	Board_MemInit();
	KernelDebugInit();
	VFS_ModuleInit();			//虚拟文件系统

	VFS_DEVFS_ModuleInit();		//设备文件系统
	BLKDEV_MouduleInit();		//块设备.

#ifndef	BOOT
	ProgressModuleInit();       //进程模块初始化(如果需要动态加载APP功能).
#endif
	////用户设备初始化
	__FileSystemInit();
	__DriversInit();

}

/*=========================================================================================*/
