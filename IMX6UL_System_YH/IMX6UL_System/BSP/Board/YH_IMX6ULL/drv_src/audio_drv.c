#include	"BSP.h"
#include	"Kernel.h"
#include	"fsl_clock.h"
#include	"fsl_sai.h"
#include	"fsl_sai_sdma.h"
#include	"emXGUI_Arch.h"

/*=========================================================================================*/

/*=========================================================================================*/

static u8 *play_next=NULL;
static u8 *play_buf1=NULL;
static u8 *play_buf2=NULL;
static int play_size=0;

static volatile u8  play_idx=0;

static GUI_SEM *sem_rdy=NULL;
/*=========================================================================================*/
//void WM8960_Init(const struct i2c_ops *iic, wm8960_config_t *config);

void 	SAI1_PortInit(void);

void	SAI1_Init(void);
void	SAI1_DMAConfig(sai_word_width_t bps,sai_sample_rate_t sample_rate,sai_sdma_callback_t cb);

void	SAI1_Start(const u8 *buf,int size);
void	SAI1_Stop(void);
void	SAI1_DMAStop(void);

/*=========================================================================================*/

static void playback(I2S_Type *base, sai_sdma_handle_t *handle, status_t status, void *userData)
{
   // isFinished = true;

	switch(play_idx)
	{
	    case 0:
    	    play_idx =1;
    	    SAI1_Start(play_buf2,play_size);

    	    play_next =play_buf1;
    	    GUI_SemPost(sem_rdy);
    	    break;
    	    ////

	    case 1:
    	    play_idx =0;
       	    SAI1_Start(play_buf1,play_size);

       	    play_next =play_buf2;
       	 GUI_SemPost(sem_rdy);
    	    break;
    	    ////
    	default:
    	    break;

	}

}


void audio_hard_init(void)
{
	SAI1_PortInit();
#if 0
	 WM8960_Init(&I2C_P1_16_P1_17,NULL);

	 WM8960_SetDataRoute(kWM8960_RoutePlayback);
	 WM8960_SetProtocol(kWM8960_BusI2S);
	 WM8960_SetMasterSlave(FALSE);

	 WM8960_SetVolume(kWM8960_ModuleDAC,0x00);        //0-FF
	 WM8960_SetVolume(kWM8960_ModuleHP,0x7F);        //0-7F
	 WM8960_SetVolume(kWM8960_ModuleSpeaker,0x7F);   //0-7F

	 WM8960_SetModule(kWM8960_ModuleLineOut, true);
	 WM8960_SetModule(kWM8960_ModuleDAC, true);
	 WM8960_SetModule(kWM8960_ModuleVREF,true);
	 WM8960_SetModule(kWM8960_ModuleHP,true);
	 WM8960_SetModule(kWM8960_ModuleSpeaker,true);
#endif
	 SAI1_Init();

}

void audio_hard_deinit(void)
{
#if 0
	WM8960_SetVolume(kWM8960_ModuleDAC,0x00);        //0-FF
	WM8960_SetVolume(kWM8960_ModuleHP,0x00);        //0-7F
	WM8960_SetVolume(kWM8960_ModuleSpeaker,0x00);   //0-7F
#endif
	SAI1_DMAStop();
	SAI1_Stop();
#if 0
	WM8960_SetModule(kWM8960_ModuleHP,false);
	WM8960_SetModule(kWM8960_ModuleSpeaker,false);
#endif
	GUI_SemPost(sem_rdy);
}

void audio_format_init(u32 bps,u32 sample_rate,u32 size)
{
	SAI1_DMAStop(); //ÏÈÒªÔÝÍ£DMA!

	play_idx =0;
	play_size =size;
	play_buf1 =dma_mem_alloc(size);
	play_buf2 =dma_mem_alloc(size);
	memset(play_buf1,0,size);
	memset(play_buf2,0,size);

	if(bps==16)
	{
		//DebugPrintf("I2S2_Init 16bit.\r\n");

//		WM8960_ConfigDataFormat(1000,sample_rate,16);
		SAI1_DMAConfig(kSAI_WordWidth16bits,sample_rate,playback);
	}
	else if(bps==24)
	{
		//DebugPrintf("I2S2_Init 24bit.\r\n");

//		WM8960_ConfigDataFormat(1000,sample_rate,24);
		SAI1_DMAConfig(kSAI_WordWidth24bits,sample_rate,playback);
	}

}

void audio_format_deinit(void)
{
	SAI1_DMAStop();
	dma_mem_free(play_buf1);
	dma_mem_free(play_buf2);
}

void* audio_wait_buffer(int wait_ms)
{
	GUI_SemWait(sem_rdy,5000);
	if(play_idx==0)
	{
		return	play_buf2;
	}
	else
	{
		return	play_buf1;
	}

}


static const u8 vol_tbl[16]={0x00,0x40,0x60,0x80,0xA0,0xB0,0xC0,0xC8,0xCC,0xD0,0xD8,0xE0,0xE4,0xE8,0xEF,0xFF};

void audio_set_volume(u8 l,u8 r) //0-255
{
	u8 i,vol;

	i =(l+r)>>1;
	i >>= 4;
	vol =vol_tbl[i];

//	WM8960_SetVolume(kWM8960_ModuleDAC,vol);        //0-FF
}

void audio_start(void)
{
	SAI1_Start(play_buf1,play_size);
//	SAI1_Start(play_buf1,play_size);
}

void audio_stop(void)
{
	SAI1_DMAStop();
//	SAI1_Stop();
}

void	Audio_DevInit(void)
{
	sem_rdy =SYS_sem_create(1,1,NULL);
	//audio_hard_init();
}

/*=========================================================================================*/
