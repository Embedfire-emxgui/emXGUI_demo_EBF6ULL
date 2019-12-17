#ifndef		__BOARD_H___
#define		__BOARD_H___

#ifdef	__cplusplus
extern "C"{
#endif

#include	"def.h"
#include	"my_iic.h"
#include	"X_GUI_Drv/GUI_Drv_Cfg.h"


#include	"fsl_common.h"

#include	<Common/drv_misc/elcdif_drv.h>
//#include	"Common/inc/my_uart.h"
//#include	"Components/NAU8822/NAU8822.h"
#include	"Components/RX8010/RX8010.h"


/*=========================================================================================*/

#define	CP437

#ifndef	BOOT
#define	CP936
#endif

/*=========================================================================================*/

#define	KB	(1024)
#define	MB	(1024*KB)

/*=========================================================================================*/

typedef struct
{
	u8 WB_space[128*MB];
	u8 WT_space[(128-1)*MB];
}sdram_space_t;

typedef struct //WB内存区.
{
	u8 sys[4*MB];    //内核运行空间.
	u8 kmem[2*MB];   //kmalloc动态内存区.
	u8 vmem[16*MB];  //vmalloc动态内存区.
}sdram_wb_t;

typedef struct //WT内存区.
{
	u8 smem[1*MB];   //静态DMA内存区.
	u8 ttmem[1*MB];  //MMU转换表内存区.
	u8 pfmem[32*MB]; //APP页面内存区.
	u8 dmem[16*MB];  //动态DMA内存区.
}sdram_wt_t;


#define	SDRAM_PA_BASE		0x80000000
#define	SDRAM_SIZE			(256*MB) //256MB

#define	SDRAM_WB_PA_BASE	(SDRAM_PA_BASE + FPOS(sdram_space_t,WB_space))
#define	SDRAM_WB_SIZE		FSIZE(sdram_space_t,WB_space)
#define	SDRAM_WB_VA_BASE	SDRAM_WB_PA_BASE

#define	SDRAM_WT_PA_BASE	(SDRAM_PA_BASE + FPOS(sdram_space_t,WT_space))
#define	SDRAM_WT_SIZE		FSIZE(sdram_space_t,WT_space)
#define	SDRAM_WT_VA_BASE	SDRAM_WT_PA_BASE

/*=========================================================================================*/

/*=========================================================================================*/
#define	Fin	(24*MHZ)
////NandFlash
#define		K9G4G08U

/*=========================================================================================*/

#define	OCSRAM_PA_BASE				0x00900000

#define	SFR_PA_BASE					0x02000000
#define	SFR_PA_END					0x02200000
#define	SFR_SIZE					(SFR_PA_END-SFR_PA_BASE) //2MB


#define	KERNEL_VA_BASE				(SDRAM_WB_VA_BASE + FPOS(sdram_wb_t,sys))

#define	KMEM_VA_BASE				(SDRAM_WB_VA_BASE + FPOS(sdram_wb_t,kmem))
#define	KMEM_SIZE				    FSIZE(sdram_wb_t,kmem)

#define	VMEM_VA_BASE				(SDRAM_WB_VA_BASE + FPOS(sdram_wb_t,vmem))
//#define	VMEM_SIZE				    FSIZE(sdram_wb_t,vmem)
#define	VMEM_SIZE					(FSIZE(sdram_space_t,WB_space) - FPOS(sdram_wb_t,vmem)-256)


#define	DMA_BUFFER_PA_BASE			(SDRAM_WT_PA_BASE + FPOS(sdram_wt_t,dmem))
//#define	DMA_BUFFER_SIZE				FSIZE(sdram_wt_t,dmem)
#define	DMA_BUFFER_SIZE				(FSIZE(sdram_space_t,WT_space) - FPOS(sdram_wt_t,dmem)-256)

#define	DMA_BUFFER_VA_BASE			(SDRAM_WT_VA_BASE + FPOS(sdram_wt_t,dmem))


#define	PAGE_FRAME_PA_BASE			(SDRAM_WT_PA_BASE + FPOS(sdram_wt_t,pfmem))
#define	PAGE_FRAME_MEM_SIZE			FSIZE(sdram_wt_t,pfmem)
#define	PAGE_FRAME_VA_BASE			(SDRAM_WT_VA_BASE + FPOS(sdram_wt_t,pfmem))

#define	MMU_TT_PA_BASE				(SDRAM_WT_PA_BASE + FPOS(sdram_wt_t,ttmem))
#define	MMU_TT_SIZE					FSIZE(sdram_wt_t,ttmem)
#define	MMU_TT_VA_BASE				(SDRAM_WT_VA_BASE + FPOS(sdram_wt_t,ttmem))

#define	RAM_PA_LAST_1MB				(SDRAM_PA_BASE+SDRAM_SIZE-(1*MB))


#define	GIC_PA_BASE					0x00A00000

#define	OCSRAM_VA_BASE				0xE0900000
#define	GIC_VA_BASE					0xE0A00000
#define	SFR_VA_BASE					0xE2000000



#define	SFR_PA2VA(pa)		(((u32)pa-(u32)SFR_PA_BASE) + (u32)SFR_VA_BASE)

#define	DMA_MEM_VA2PA(va)	(((u32)va-(u32)DMA_BUFFER_VA_BASE) + (u32)DMA_BUFFER_PA_BASE)
#define	MTT_VA2PA(va)		(((u32)va-(u32)MMU_TT_VA_BASE) + (u32)MMU_TT_PA_BASE)
#define	PF_PA2VA(pa)		(((u32)pa-(u32)PAGE_FRAME_PA_BASE) + (u32)PAGE_FRAME_VA_BASE)

/*=========================================================================================*/
#define BOARD_USDHC1_BASEADDR USDHC1
#define BOARD_USDHC2_BASEADDR USDHC2


//#define BOARD_USDHC1_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc1Div) + 1U))
//#define BOARD_USDHC2_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U))

//#define BOARD_SD_HOST_BASEADDR BOARD_USDHC1_BASEADDR
//#define BOARD_SD_HOST_CLK_FREQ BOARD_USDHC1_CLK_FREQ

#define BOARD_USDHC_CD_GPIO_BASE GPIO1
#define BOARD_USDHC_CD_GPIO_PIN 19
#define BOARD_USDHC_CD_PORT_IRQ GPIO1_Combined_16_31_IRQn
#define BOARD_USDHC_CD_PORT_IRQ_HANDLER GPIO_IRQ_Handler



/*=========================================================================================*/

extern const char TargetBoard_IO[];

void	Board_ModuleInit(void);

void* dma_mem_alloc(u32 size);
void  dma_mem_free(void *p);

void BoardMem_Init(void);

void KernelMemMap_Init(u32 *tt_base);
void UserMemMap_Init(u32 *tt_base);

////////////////
volatile BOOL USBD_HardIsCon;
volatile BOOL USBD_DevIsCon;

extern u32 UPLL_Clk,APLL_Clk,SYS_Clk,PCLK,CPU_Clk;

/*=========================================================================================*/

extern const struct uart_ops UART0_ops;

////////////////

extern const struct	i2c_ops I2C_P1_28_P1_29;
extern const struct	i2c_ops I2C_P1_30_P1_31;

/*=========================================================================================*/
#include	"IMX6UL/fsl_common.h"
#include	"IMX6UL/core_ca7.h"
#include	"IMX6UL/core_ca.h"

#include	"IMX6UL/fsl_clock.h"
#include	"IMX6UL/fsl_iomux.h"
#include	"IMX6UL/interrupt.h"

#include	"IMX6UL/fsl_cache.h"
#include	"IMX6UL/fsl_gpio.h"
#include	"IMX6UL/fsl_epit.h"
#include	"IMX6UL/fsl_elcdif.h"




#ifdef	__cplusplus
}
#endif

#endif	/*__BOARD_H___*/
