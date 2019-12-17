
#include "BSP.h"
#include "fsl_sai.h"
#include "fsl_sai_sdma.h"

/*=========================================================================================*/
#define	DEMO_CODEC_WM8960

#define	PRINTF(x,...)

#define DEMO_SAI 			I2S1
#define DEMO_SAI_CHANNEL 	(0)
//#define DEMO_SAI_BITWIDTH 	(kSAI_WordWidth16bits)
#define DEMO_SAI_IRQ 		SAI1_IRQn
#define DEMO_I2C 			I2C2

/* Select Audio/Video PLL (786.48 MHz) as sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_SELECT 			(2U)
/* Clock pre divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER 		(0U)
/* Clock divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_DIVIDER 			(15U)

//#define DEMO_SAI_CLK_FREQ (CLOCK_GetFreq(kCLOCK_AudioPllClk) / ((CLOCK_GetDiv(kCLOCK_Sai1PreDiv) + 1U) * (CLOCK_GetDiv(kCLOCK_Sai1Div) + 1U)))
//#define DEMO_I2C_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

#define EXAMPLE_DMA 	SDMAARM
#define EXAMPLE_CHANNEL 1   //(1)
#define SAI_TX_SOURCE 	38 //(38)

/*=========================================================================================*/

static const clock_audio_pll_config_t g_ccmConfigAudioPll =
{
    .loopDivider = 30, .postDivider = 2, .numerator = 72, .denominator = 100
};

AT_NONCACHEABLE_SECTION_ALIGN(static sai_sdma_handle_t txHandle, 4)  = {0};
AT_NONCACHEABLE_SECTION_ALIGN(static sdma_handle_t dmaHandle, 4)     = {0};
AT_NONCACHEABLE_SECTION_ALIGN(static sdma_context_data_t context, 4) = {0};

static volatile bool isFinished = false;
static  sai_transfer_t xfer;
/*=========================================================================================*/
#if 0
static	void SAI_ClockConfig(int sr)
{
	int a;
	/*
	 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
	 *                              = 24 * (32 + 77/100)
	 *                              = 786.48 MHz
	 */
	clock_audio_pll_config_t pll_cfg = {
	    .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
	    .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
	    .numerator = 77,    /* 30 bit numerator of fractional loop divider. */
	    .denominator = 100, /* 30 bit denominator of fractional loop divider */
	};

	a =sr/1000;

	switch(a)
	{
		case	11:
		case	22:
		case	44:
			pll_cfg.loopDivider =37;
			pll_cfg.postDivider =1;
			pll_cfg.numerator   =632;
			pll_cfg.denominator =1000;
			CLOCK_InitAudioPll(&pll_cfg);

		    CLOCK_SetMux(kCLOCK_Sai1Mux, DEMO_SAI1_CLOCK_SOURCE_SELECT);
		    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER);
		    CLOCK_SetDiv(kCLOCK_Sai1Div, DEMO_SAI1_CLOCK_SOURCE_DIVIDER);

			break;
			////

		case	8:
		case	16:
		case	32:
		case	48:
		case	96:

			pll_cfg.loopDivider =32;
			pll_cfg.postDivider =1;
			pll_cfg.numerator   =768;
			pll_cfg.denominator =1000;
			CLOCK_InitAudioPll(&pll_cfg);

		    CLOCK_SetMux(kCLOCK_Sai1Mux, DEMO_SAI1_CLOCK_SOURCE_SELECT);
		    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER);
		    CLOCK_SetDiv(kCLOCK_Sai1Div, DEMO_SAI1_CLOCK_SOURCE_DIVIDER);
			break;

	}

    /* Enable SPBA */
    CLOCK_EnableClock(kCLOCK_Spba);

}
#endif

#if 1
static void SAI_ClockConfig(int sr)
{


    /* Enable PLL4 368.64M */
    CLOCK_InitAudioPll(&g_ccmConfigAudioPll);
    /* Set SAI source to PLL4*/
    CLOCK_SetMux(kCLOCK_Sai1Mux, 2);
    /* Set SAI pre-divider to 6 */
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 5U);
    /* Set SAI divider to 10 */
    CLOCK_SetDiv(kCLOCK_Sai1Div, 9U);

    /* Enable SPBA */
    CLOCK_EnableClock(kCLOCK_Spba);
}
#endif

static u32 SAI_GetClockFreq(void)
{
	volatile u32 freq;
	freq = CLOCK_GetFreq(kCLOCK_AudioPllClk);
	freq = freq / ((CLOCK_GetDiv(kCLOCK_Sai1PreDiv) + 1U) * (CLOCK_GetDiv(kCLOCK_Sai1Div) + 1U));
	return freq;
}

static void SAI_EnableMclkOutput(bool enable)
{
    if (enable)
    {
        IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK;
    }
    else
    {
        IOMUXC_GPR->GPR1 &= (~IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK);
    }
}

#if 0
static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++)
    {
        __NOP();
    }
}
#endif

void BOARD_I2C_ReleaseBus(void)
{
#if 0
    uint8_t i = 0;
    gpio_pin_config_t pin_config;

    pin_config.direction = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    pin_config.interruptMode = kGPIO_NoIntmode;

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++)
    {
        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
#endif
}

static void callback(I2S_Type *base, sai_sdma_handle_t *handle, status_t status, void *userData)
{
    isFinished = true;
}

static void SAI_UserIRQHandler(void)
{
	volatile u32 sta;
	sta =SAI_TxGetStatusFlag(DEMO_SAI);

    SAI_TxClearStatusFlags(DEMO_SAI, kSAI_WordStartFlag|kSAI_FIFOErrorFlag|kSAI_SyncErrorFlag);
   // SAI_TxClearStatusFlags(DEMO_SAI, kSAI_FIFOErrorFlag);
}

#if 0
/*!
 * @brief Main function
 */
static void	SAI_Test(void)
{
    sai_config_t config;
    uint32_t mclkSourceClockHz = 0U;
    sai_transfer_format_t format;
    sai_transfer_t xfer;
    sdma_config_t dmaConfig = {0};

    uint32_t i2cSourceClock;
    uint32_t temp = 0;


    BOARD_EnableSaiMclkOutput(true);

    /* Enable PLL4 368.64M */
    CLOCK_InitAudioPll(&g_ccmConfigAudioPll);
    /* Set SAI source to PLL4*/
    CLOCK_SetMux(kCLOCK_Sai2Mux, 2);
    /* Set SAI pre-divider to 6 */
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, 5U);
    /* Set SAI divider to 10 */
    CLOCK_SetDiv(kCLOCK_Sai2Div, 9U);

    /* Enable SPBA */
    CLOCK_EnableClock(kCLOCK_Spba);

    /* Install IRQ Handler */
    SystemInitIrqTable();

    /* Install user defined SAI IRQ handler */
    //SystemInstallIrqHandler(SAI2_IRQn, (system_irq_handler_t)(uint32_t)SAI_UserIRQHandler, NULL);
    SetIRQHandler(SAI2_IRQn,SAI_UserIRQHandler);

    PRINTF("SAI SDMA example started!\n\r");

    /* Create SDMA handle */
    SDMA_GetDefaultConfig(&dmaConfig);
    SDMA_Init(EXAMPLE_DMA, &dmaConfig);
    SDMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL, &context);
    SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL, 2);

    /* Init SAI module */
    /*
     * config.masterSlave = kSAI_Master;
     * config.mclkSource = kSAI_MclkSourceSysclk;
     * config.protocol = kSAI_BusLeftJustified;
     * config.syncMode = kSAI_ModeAsync;
     * config.mclkOutputEnable = true;
     */
    SAI_TxGetDefaultConfig(&config);
    SAI_TxInit(DEMO_SAI, &config);

    /* Configure the audio format */
    format.bitWidth = kSAI_WordWidth16bits;
    format.channel = 0U;
    format.sampleRate_Hz = kSAI_SampleRate16KHz;
#if (defined FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER && FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) || \
    (defined FSL_FEATURE_PCC_HAS_SAI_DIVIDER && FSL_FEATURE_PCC_HAS_SAI_DIVIDER)
    format.masterClockHz = OVER_SAMPLE_RATE * format.sampleRate_Hz;
#else
    format.masterClockHz = DEMO_SAI_CLK_FREQ;
#endif
    format.protocol = config.protocol;
    format.stereo = kSAI_Stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

    /* Configure Sgtl5000 I2C */
   // codecHandle.base = DEMO_I2C;
    //codecHandle.i2cHandle = &i2cHandle;
   // i2cSourceClock = DEMO_I2C_CLK_FREQ;


    SAI_TransferTxCreateHandleSDMA(DEMO_SAI, &txHandle, callback, NULL, &dmaHandle, SAI_TX_SOURCE);

    mclkSourceClockHz = DEMO_SAI_CLK_FREQ;
    SAI_TransferTxSetFormatSDMA(DEMO_SAI, &txHandle, &format, mclkSourceClockHz, format.masterClockHz);

    /* Enable SAI interrupt to clear the FIFO error */
    EnableIRQ(SAI2_IRQn);
    SAI_TxEnableInterrupts(DEMO_SAI, kSAI_FIFOErrorInterruptEnable);

    /*  xfer structure */
#if 0
    temp = (uint32_t)music;
    xfer.data = (uint8_t *)temp;
    xfer.dataSize = MUSIC_LEN;
#endif
    SAI_TransferSendSDMA(DEMO_SAI, &txHandle, &xfer);

    /* Wait until finished */
    while (isFinished != true)
    {
    }

#if 0
#if defined(DEMO_CODEC_WM8960)
    WM8960_Deinit(&codecHandle);
#elif defined(DEMO_CODEC_DA7212)
#else
    SGTL_Deinit(&codecHandle);
#endif

#if defined(FSL_FEATURE_SOC_LPI2C_COUNT) && (FSL_FEATURE_SOC_LPI2C_COUNT)
    LPI2C_MasterDeinit(DEMO_I2C);
#else
    I2C_MasterDeinit(DEMO_I2C);
#endif
#endif

    PRINTF("\n\r SAI SDMA example finished!\n\r ");

}

#endif

/*=========================================================================================*/
/*=========================================================================================*/
static sai_config_t config;
static sai_transfer_format_t format;
static sdma_config_t dmaConfig = {0};

void	SAI1_Init(void)
{
	 //uint32_t mclkSourceClockHz = 0U;
	// sai_transfer_t xfer;

	 uint32_t i2cSourceClock;
	 uint32_t temp = 0;

	memset(&txHandle,0,sizeof(sai_sdma_handle_t));

	 SAI_EnableMclkOutput(true);

	 SAI_ClockConfig(44100);

	    /* Install IRQ Handler */
	    //SystemInitIrqTable();

	    /* Install user defined SAI IRQ handler */
	    //SystemInstallIrqHandler(SAI2_IRQn, (system_irq_handler_t)(uint32_t)SAI_UserIRQHandler, NULL);


	  PRINTF("SAI SDMA example started!\n\r");

	  /* Create SDMA handle */
	  SDMA_GetDefaultConfig(&dmaConfig);
	  SDMA_Init(EXAMPLE_DMA, &dmaConfig);
	  SDMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL, &context);
	  SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL, 2);
#if 0
	  /* Create SDMA handle */
	  SDMA_GetDefaultConfig(&dmaConfig);
	  SDMA_Init(EXAMPLE_DMA, &dmaConfig);
	  SDMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL, &context);
	  SDMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_CHANNEL, 2);
#endif
	    /* Init SAI module */
	    /*
	     * config.masterSlave = kSAI_Master;
	     * config.mclkSource = kSAI_MclkSourceSysclk;
	     * config.protocol = kSAI_BusLeftJustified;
	     * config.syncMode = kSAI_ModeAsync;
	     * config.mclkOutputEnable = true;
	     */
	  SAI_TxGetDefaultConfig(&config);
	  SAI_TxInit(DEMO_SAI, &config);

	  DisableIRQ(DEMO_SAI_IRQ);
	  SetIRQHandler(DEMO_SAI_IRQ,SAI_UserIRQHandler);
	  SAI_TxEnableInterrupts(DEMO_SAI, kSAI_FIFOErrorInterruptEnable);
	  EnableIRQ(DEMO_SAI_IRQ);

}

void	SAI1_DMAConfig(sai_word_width_t bps,sai_sample_rate_t sample_rate,sai_sdma_callback_t cb)
{
	//sai_config_t config;
	uint32_t mclkSourceClockHz = 0U;
	//sai_transfer_format_t format;

	//sdma_config_t dmaConfig = {0};

	uint32_t i2cSourceClock;
	uint32_t temp = 0;

	SAI_ClockConfig(sample_rate);

	    /* Configure the audio format */
	    format.bitWidth = bps;//kSAI_WordWidth16bits;
	    format.channel = 0U;
	    format.sampleRate_Hz = sample_rate;//kSAI_SampleRate16KHz;

	#if (defined FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER && FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) || \
	    (defined FSL_FEATURE_PCC_HAS_SAI_DIVIDER && FSL_FEATURE_PCC_HAS_SAI_DIVIDER)
	    format.masterClockHz = OVER_SAMPLE_RATE * format.sampleRate_Hz;
	#else
	    format.masterClockHz = SAI_GetClockFreq();
	#endif

	    format.protocol = config.protocol;
	    format.stereo = kSAI_Stereo;
	#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
	    format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
	#endif

	    /* Configure Sgtl5000 I2C */
	   // codecHandle.base = DEMO_I2C;
	    //codecHandle.i2cHandle = &i2cHandle;
	   // i2cSourceClock = DEMO_I2C_CLK_FREQ;

	    memset(&txHandle,0,sizeof(sai_sdma_handle_t));
	    SAI_TransferTxCreateHandleSDMA(DEMO_SAI, &txHandle, cb, NULL, &dmaHandle, SAI_TX_SOURCE);

	    mclkSourceClockHz = SAI_GetClockFreq();
	    SAI_TransferTxSetFormatSDMA(DEMO_SAI, &txHandle, &format, mclkSourceClockHz, format.masterClockHz);


	    /*  xfer structure */
	#if 0
	    temp = (uint32_t)music;
	    xfer.data = (uint8_t *)temp;
	    xfer.dataSize = MUSIC_LEN;
	#endif
}



void	SAI1_Start(const u8 *buf,int size)
{
#if 1
	uint32_t temp;

	/*  xfer structure */
	temp = (uint32_t)buf;//music;

	xfer.data = (uint8_t *)buf;
	xfer.dataSize = size;//MUSIC_LEN;
	SAI_TransferSendSDMA(DEMO_SAI, &txHandle, &xfer);
#endif

}

void	SAI1_Stop(void)
{
	SAI_Deinit(DEMO_SAI);
}

void	SAI1_DMAStop(void)
{
	if(txHandle.dmaHandle!=NULL)
	{
		SAI_TransferAbortSendSDMA(DEMO_SAI, &txHandle);
	}
	SAI_EnableMclkOutput(false);
}

#include "x_file.h"
static int wav_size=0;
static u8 *wav_buf=NULL;
int SAI1_Test(void)
{
	char path[128];
	  SAI1_PortInit();
	  // WM8960_Init(&I2C_P1_16_P1_17,NULL);
	  // WM8960_SetMasterSlave(FALSE);

	    if(CDlg_FileExpoler(NULL,20,30,450,400,L"File Expolorer","B:",path))
	    {
	    	X_FILE *fp;

	    	//fp =x_fopen("B:test.wav","rb");
	    	fp =x_fopen(path,"rb");
	    	if(fp!=NULL)
	    	{
	    		wav_size =1024*1024;
	    		wav_buf =dma_mem_alloc(wav_size);

	    		x_fseek(fp,5*1024*1024,SEEK_SET);
	    		x_fread(wav_buf,wav_size,1,fp);
	    		x_fclose(fp);
	    	}
	    }
	    else
	    {
	    	return 0;
	    }

	    SAI1_Init();
	    SAI1_DMAConfig(kSAI_WordWidth16bits,kSAI_SampleRate44100Hz,callback);
	    SAI1_Start(wav_buf,1024*1024);
	    while (isFinished != true)
	    {
	    	GUI_msleep(50);
	    }
	    isFinished=false;
	    SAI1_DMAStop();
	    SAI1_Stop();

	    GUI_msleep(1000);
	    dma_mem_free(wav_buf);
}

/*=========================================================================================*/
