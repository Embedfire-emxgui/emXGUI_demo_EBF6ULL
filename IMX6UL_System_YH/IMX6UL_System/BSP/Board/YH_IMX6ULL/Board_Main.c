#include	"Include.h"
#include	"BSP.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/*=========================================================================================*/

void Board_OnTick(void)
{
//	ALCO_Dev_OnTimer();
//	lcd_handler();
}
extern irq_hdlr_t g_interrupt_handlers[IMX_INTERRUPT_COUNT];
/*Provides own implementation of vApplicationIRQHandler() to call SystemIrqHandler()
 *directly without saving the FPU registers on interrupt entry.
 */
void vApplicationIRQHandler(uint32_t ulICCIAR)
{
	if ((ulICCIAR == 1023) || (ulICCIAR >= NUMBER_OF_INT_VECTORS))
	{
		return;
	}

	irq_hdlr_t isr = g_interrupt_handlers[ulICCIAR];

	if (isr)
	{
		isr();
	}
	//	GIC_IRQHandler();
}

/*=========================================================================================*/

void Board_HardInit(void);
void KernelStartup(void);
static	void	GUI_Thread(void *argv);

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hello_task(void *pvParameters);

void Board_Startup(void)
{
  	Board_HardInit();
#if 0
    KernelStartup();	//启动操作系统(该函数不会返回，多任务环境开始)
#else
    if (xTaskCreate(GUI_Thread, "gui", 1024*4/4, NULL, 10, NULL) != pdPASS)
        {
//            PRINTF("Task creation failed!.\r\n");
            while (1)
                ;
        }

//    if (xTaskCreate(hello_task, "gui", 1024*1/4, NULL, 6, NULL) != pdPASS)
//            {
//    //            PRINTF("Task creation failed!.\r\n");
//                while (1)
//                    ;
//            }

        vTaskStartScheduler();
        for (;;)
            ;
#endif
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
    for (;;)
    {
    	GPIO_PinWrite(GPIO5,3,1);
    	vTaskDelay(2000);
    	GPIO_PinWrite(GPIO5,3,0);
    	vTaskDelay(2000);
//        PRINTF("Hello world.\r\n");
//        vTaskSuspend(NULL);
    }
}

/*=========================================================================================*/


BOOL	GUI_Startup(void);
static	void	GUI_Thread(void *argv)
{
    argv = NULL;

//    SetThreadPriority(NULL,+2);

    //touch_handler(NULL);
    //SD_Test();
   // adc_tsc_test();
   // gpio_test();
    //USBD_MSC_Execu();

	while(1)
	{
		GUI_Startup();
	}
}

/*=========================================================================================*/

void OEM_Init(void)
{

#ifdef	BOOT
	BootMain();
#else
	SYS_thread_create(GUI_Thread,NULL,8192,NULL,0);
#endif

}

/*=========================================================================================*/


int cap_test(u8 *u8FrameBufPtr);

void usbh_hid_test(void);
void USBD_MSC_Test (void);
void udcInit(void);

static int app_thread(void *argv)
{
	X_FILE *fp;
	u32 fsize;
	u8 *buf;

	Board_AppMain(0,GUI_YSIZE-32,GUI_XSIZE,400,L"YH-iMX6ULL");
	//SYS_msleep(1000);
	//GUI_DEMO_RectSpeed();
	//CDlg_FileExpoler(NULL,20,30,450,400,L"File Expolorer","B:",NULL);

	//GUI_Demo();
	//ISL1208_Test();
	//mjpg_play("B:test_mjpg.avi");
	//mjpg_play("B:test_h264.avi");
	//H264_play("B:test_h264.avi");
	//AudioPlayer(L"AudioPlayer",10,20,360,320);

#if 0
	////
	fp =x_fopen("B:Image/test.jpg","rb");
	if(fp!=NULL)
	{
		fsize =x_fsize(fp);
		buf =vmalloc(fsize);
		x_fread(buf,fsize,1,fp);
		x_fclose(fp);
		HW_JPEG_Decode(buf,fsize);

	}
#endif
	return 0;
}

void	GUI_UserAppStart(void)
{
	int	i;

	GUI_msleep(500);
	BL_PWM_Set(30);

//	GUI_DEMO_RectSpeed();
	GUI_Thread_Create(app_thread,NULL,16*KB,NULL,0);
	//cam_test_main();
	//enet_test_main();
	//ECSPI_Interrupt_MasterTest();
	//ECSPI_DMA_MasterTest();
	SAI1_Test();
	//usbd_vcom_main();
	//UART1_Test();
//	sd_main();

	//USBD_VCOM_Test();
	//udcInit();
	//adc_tsc_test();
	//usbh_hid_test();
//	USBD_MSC_Test();
//	USBD_MSC_WinMain("RAMDisk");
	//sdmmc0_test();
	//sdmmc1_test();
	//mx29lv800_test();
  //Test_PlaySound();
	//cap_test(LCD_FB_PA);

#if 0
    if(GetKeyState(VK_RIGHT))
    if(GetKeyState(VK_RETURN))
    {
    	SetAutoRun("B:AutoRun.ini");
	}

	ExecuAutoRun("B:AutoRun.ini");
#endif

//	SYS_execu("B:apps\AppTemplet.app",NULL,NULL);
//	SYS_execu("B:apps\Fit240\Fit240.app",NULL,NULL);
//	SYS_execu("B:apps\Fish.app",NULL,NULL);

//	GUI_DEMO_StretchBlt();

//	BMP_Test();



	//Sleep(2000);

//	SYS_thread_create(gpio_test,NULL,4096,NULL);

//	nRF24L01_Test();
	//print_screen();
//	lcd_test();
//	G2D_Test();

//	lwIP_Telnet();
//	Listview_Test();
//	AudioPlayerStartup();


	CPP_Entry();
}

/*=========================================================================================*/

