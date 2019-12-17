

/* Includes ------------------------------------------------------------------*/
#include "BSP.h"
#include "bsp_csi.h"
#include "fsl_ov5640.h"

void BOARD_InitCSIPins(void);
void BOARD_PullCameraResetPin(bool pullUp);
void BOARD_PullCameraPowerDownPin(bool pullUp);
/*=========================================================================================*/

static void (*cbHandler)(void) =NULL;

/*=========================================================================================*/

/*=========================================================================================*/

uint32_t activeFrameAddr;
uint32_t inactiveFrameAddr;
static csi_private_data_t csiPrivateData;

/* OV5640连接到CSI */
static csi_resource_t csiResource =
{
    .csiBase = CSI,
};

static camera_receiver_handle_t cameraReceiver =
{
    .resource = &csiResource,
	.ops = &csi_ops,
	.privateData = &csiPrivateData,
};

/**
  * @brief  摄像头中断处理函数
  * @param  无
  * @retval 无
  */
void CSI_IRQHandler(void)
{
	mk_isr_enter();
    CSI_DriverIRQHandler();
    mk_isr_exit();
}


static ov5640_resource_t ov5640Resource = {
  //  .sccbI2C = OV5640_I2C,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
   // .inputClockFreq_Hz = 24000000,
};

static camera_device_handle_t cameraDevice = {
    .resource = &ov5640Resource,
	.ops = &ov5640_ops,
};

static void BOARD_InitCameraResource(void)
{
#if 0
    lpi2c_master_config_t masterConfig;
    uint32_t sourceClock;

    LPI2C_MasterGetDefaultConfig(&masterConfig);
		/*设置I2C时钟为400KHz*/
    masterConfig.baudRate_Hz = 400000;
    masterConfig.debugEnable = true;
    masterConfig.ignoreAck = true;

    /*LPI2C时钟配置 */
    /*
     * LPI2C 时钟源选择:
     *  0: pll3_60m
     *  1: OSC clock
     */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 1);
    /*
     * LPI2C 分频系数.
     *  0b000000: Divide by 1
     *  0b111111: Divide by 2^6
     */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);

    /* LPI2C 时钟源为 OSC */
    sourceClock = CLOCK_GetOscFreq();

    LPI2C_MasterInit(OV5640_I2C, &masterConfig, sourceClock);
#endif

#if 0
    /* 初始化摄像头的PDN和RST引脚 */
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalOutput, 1,
    };

    GPIO_PinInit(GPIO1, 0, &pinConfig);
	GPIO_PinInit(GPIO1, 1, &pinConfig);
#endif

}


#include "BSP.h"

static sem_obj *sem_cam=NULL;
static u8* cam_fb[APP_FRAME_BUFFER_COUNT];

static void cam__callback(camera_receiver_handle_t *handle, status_t status, void *userData)
{
	if(cbHandler!=NULL)
	{
		cbHandler();
	}
	SYS_sem_post(sem_cam);
}


/**
  * @brief  配置OV5640
  * @param  None
  * @retval None
  */
static void Camera_Init(void)
{
		/* 初始化摄像头引脚 */
		BOARD_InitCSIPins();

		/* 初始化摄像头的I2C及控制引脚 */
		BOARD_InitCameraResource();

#if 0
    elcdif_rgb_mode_config_t lcdConfig = {
        .panelWidth = APP_IMG_WIDTH,
        .panelHeight = APP_IMG_HEIGHT,
        .hsw = APP_HSW,
        .hfp = APP_HFP,
        .hbp = APP_HBP,
        .vsw = APP_VSW,
        .vfp = APP_VFP,
        .vbp = APP_VBP,
        .polarityFlags = APP_POL_FLAGS,
        .pixelFormat = kELCDIF_PixelFormatRGB565,
        .dataBus = APP_LCDIF_DATA_BUS,
    };
#endif

#if 0
    const camera_config_t cameraConfig =
    {
        .pixelFormat   = kVIDEO_PixelFormatRGB565,
        .bytesPerPixel = APP_BPP,
        .resolution    = FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT),
        .frameBufferLinePitch_Bytes = APP_IMG_WIDTH * APP_BPP,
        .interface = kCAMERA_InterfaceGatedClock,
        .controlFlags = kCAMERA_VsyncActiveLow | kCAMERA_HrefActiveLow | kCAMERA_DataLatchOnRisingEdge,
        .framePerSec = 15,
    };
#endif

    const camera_config_t cameraConfig = {
        .pixelFormat = kVIDEO_PixelFormatRGB565,
        .bytesPerPixel = APP_BPP,
        .resolution = FSL_VIDEO_RESOLUTION(APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT),
        .frameBufferLinePitch_Bytes = APP_IMG_WIDTH * APP_BPP,
        .interface = kCAMERA_InterfaceGatedClock,
        .controlFlags = APP_CAMERA_CONTROL_FLAGS,
        .framePerSec = 30,
    };


//    memset(s_frameBuffer, 0, sizeof(s_frameBuffer));

    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, cam__callback, NULL);
    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
    CAMERA_DEVICE_Start(&cameraDevice);

    /* 将空帧缓冲区提交到缓冲区队列. */
    for (uint32_t i = 0; i < APP_FRAME_BUFFER_COUNT; i++)
    {
    	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(cam_fb[i]));

       // CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(s_frameBuffer[i]));
    }

    CAMERA_RECEIVER_Start(&cameraReceiver);

#if 0
    /*
     * LCDIF 有活动缓冲区和非活动缓冲区, 因此请在此处获取两个缓冲区.
     */
    /* 等待获取完整帧缓冲区以显示 */
    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &activeFrameAddr))
    {
    }

    /* 等待获取完整帧缓冲区以显示 */
    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &inactiveFrameAddr))
    {
    }
#endif

#if 0
    lcdConfig.bufferAddr = (uint32_t)activeFrameAddr;

    ELCDIF_RgbModeInit(APP_ELCDIF, &lcdConfig);

    ELCDIF_SetNextBufferAddr(APP_ELCDIF, inactiveFrameAddr);
    ELCDIF_RgbModeStart(APP_ELCDIF);
	  /* 打开背光 */
    GPIO_PinWrite(LCD_BL_GPIO, LCD_BL_GPIO_PIN, 1);
#endif
}

#include "emXGUI.h"
#include "Kernel.h"

BOOL	CAM_DrvInit(void)
{
	int i;

	sem_cam =SYS_sem_create(0,1,NULL);

	for(i=0;i<APP_FRAME_BUFFER_COUNT;i++)
	{
		cam_fb[i] =dma_mem_alloc(APP_CAMERA_WIDTH*APP_CAMERA_HEIGHT*2);
	}

	Camera_Init();
	return TRUE;
}

BOOL    CAM_SetSize(int w,int h)
{

}

BOOL	CAM_SyncOut(BITMAP *bm,int wait_ms)
{

	if(SYS_sem_wait(sem_cam,wait_ms)==SYS_NO_ERR)
	{
		if(CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &activeFrameAddr)==kStatus_Success)
		{
			//CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);

			bm->Format     =BM_RGB565;
			bm->Width      =APP_CAMERA_WIDTH;
			bm->Height     =APP_CAMERA_HEIGHT;
			bm->WidthBytes =APP_CAMERA_WIDTH*2;
			bm->Bits	   =(void*)activeFrameAddr;
			bm->LUT 	   =NULL;
			return TRUE;
		}
	}

	return FALSE;
}

void 	CAM_Continue(void)
{
	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);
}

void 	CAM_DrvDeinit(void)
{
	int i;

	CAMERA_RECEIVER_Stop(&cameraReceiver);
	CAMERA_DEVICE_Stop(&cameraDevice);

	CAMERA_RECEIVER_Deinit(&cameraReceiver);
	CAMERA_DEVICE_Deinit(&cameraDevice);

	SYS_sem_destroy(sem_cam);

	for(i=0;i<APP_FRAME_BUFFER_COUNT;i++)
	{
		dma_mem_free(cam_fb[i]);
	}
}

static void cam_handler(void)
{

}



