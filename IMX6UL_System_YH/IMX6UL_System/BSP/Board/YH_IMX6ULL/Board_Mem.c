
/**
  *********************************************************************
  * @file    gui_mem_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   动态内存管理接口
  *
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */
#include "x_libc.h" // x_libc里，提供了通用的内存堆管理函数(x_heap)...
#include "board.h"
#include "gui_drv.h"
#include "emXGUI_Arch.h"

/**
* @note  vmem/vmalloc/vfree 提供给应用程序自由使用，
*        通常使用一块较大的外部SRAM、SDRAM空间进行分配。
*
*        GUI_MEM_Init/Alloc/Free 提供给GUI内核对象使用，
*        通常使用控制器的内部SRAM进行分配。
*
*        GUI_GRAM_Alloc/Free 在申请液晶显存空间的时候使用，
*        本例子显存空间固定，所以没有用到
*
*/

/* VMEM内存管理 */
/* 互斥信号量 */
static GUI_MUTEX *mutex_vmem = NULL;
static GUI_MUTEX *mutex_dma  = NULL;

/* 内存堆管理句柄 */
static heap_t heap_vmem;
static heap_t heap_dma;

void GUI_DMA_Init(void);
void GUI_VMEM_Init(void);

#include "Include.h"

/*=========================================================================================*/


void KernelMemMap_Init(u32 *tt_base) //MMU OFF
{

#if 1
	mmu_map_section(tt_base,	0x00000000,				0x00000000,				4096,					MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	SDRAM_WB_VA_BASE,		SDRAM_WB_PA_BASE,	 	SDRAM_WB_SIZE>>20,		MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,CB));	//RAM WB
	mmu_map_section(tt_base,	SDRAM_WT_VA_BASE,		SDRAM_WT_PA_BASE,	 	SDRAM_WT_SIZE>>20,		MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,NCB));	//RAM WT

	mmu_map_section(tt_base,	SFR_VA_BASE,			SFR_PA_BASE,			SFR_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_CHECK,NCNB));
	mmu_map_section(tt_base,	OCSRAM_VA_BASE,			OCSRAM_PA_BASE,			1,						MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,CB));

	mmu_map_section(tt_base,	GIC_VA_BASE,			GIC_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	0xFFF00000,				RAM_PA_LAST_1MB,		1,						MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_NO_CHECK,CB));

#endif

#if 0
	mmu_map_section(tt_base,	0x00B00000U,				0x00B00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* GPV_0 PL301 */
	mmu_map_section(tt_base,	0x00C00000U,				0x00C00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* GPV_1 PL301 */
	mmu_map_section(tt_base,	0x00D00000U,				0x00D00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* cpu */
	mmu_map_section(tt_base,	0x00E00000U,				0x00E00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* per_m */
	mmu_map_section(tt_base,	0x01800000U,				0x01800000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* APBH DMA */
	mmu_map_section(tt_base,	0x02000000U,				0x02000000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* AIPS-1 */
	mmu_map_section(tt_base,	0x02100000U,				0x02100000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* AIPS-2 */
	mmu_map_section(tt_base,	0x02200000U,				0x02200000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCB)); /* AIPS-3 */
#endif

#if 0
	mmu_map_section(tt_base,	0x00000000,				0x00000000,	4096,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));

	////For Debug
	//mmu_map_section(tt_base,	RAM_PA_BASE,			RAM_PA_BASE,	 		RAM_SIZE>>20,			MMU_L1_SEC(AP_USER_RW,DOMAIN_NO_CHECK,CNB));

	mmu_map_section(tt_base,	KERNEL_VA_BASE,			RAM_PA_BASE,	 		RAM_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));	//RAM WB
	mmu_map_section(tt_base,	PAGE_FRAME_VA_BASE,		PAGE_FRAME_PA_BASE,	 	PAGE_FRAME_MEM_SIZE>>20,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));	//RAM WT
	mmu_map_section(tt_base,	SFR_VA_BASE,			SFR_PA_BASE,			SFR_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	OCSRAM_VA_BASE,			OCSRAM_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));

	mmu_map_section(tt_base,	MMU_TT_VA_BASE,			MMU_TT_PA_BASE,			MMU_TT_SIZE>>20,		MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));
	mmu_map_section(tt_base,	DMA_BUFFER_VA_BASE,		DMA_BUFFER_PA_BASE,		DMA_BUFFER_SIZE>>20,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));
	mmu_map_section(tt_base,	GIC_VA_BASE,			GIC_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	0xFFF00000,				RAM_PA_LAST_1MB,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));



	//////
#if 0
	mmu_map_section(tt_base,	0x00B00000U,				0x00B00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* GPV_0 PL301 */
	mmu_map_section(tt_base,	0x00C00000U,				0x00C00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* GPV_1 PL301 */
	mmu_map_section(tt_base,	0x00D00000U,				0x00D00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* cpu */
	mmu_map_section(tt_base,	0x00E00000U,				0x00E00000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* per_m */
	mmu_map_section(tt_base,	0x02000000U,				0x02000000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* AIPS-1 */
	mmu_map_section(tt_base,	0x02100000U,				0x02100000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* AIPS-2 */
	mmu_map_section(tt_base,	0x02200000U,				0x02200000U,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); /* AIPS-3 */
#endif

#if 0
	MMU_ConfigSection(MMU_L1Table, (const void *)0x00B00000U, 0x00B00000U, &s_mmuDevAttr); /* GPV_0 PL301 */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x00C00000U, 0x00C00000U, &s_mmuDevAttr); /* GPV_1 PL301 */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x00D00000U, 0x00D00000U, &s_mmuDevAttr); /* cpu */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x00E00000U, 0x00E00000U, &s_mmuDevAttr); /* per_m */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x01800000U, 0x01800000U, &s_mmuDevAttr); /* APBH DMA */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x02000000U, 0x02000000U, &s_mmuDevAttr); /* AIPS-1 */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x02100000U, 0x02100000U, &s_mmuDevAttr); /* AIPS-2 */
	MMU_ConfigSection(MMU_L1Table, (const void *)0x02200000U, 0x02200000U, &s_mmuDevAttr); /* AIPS-3 */
#endif

	#endif


}


void UserMemMap_Init(u32 *tt_base)
{
#if 1
	mmu_map_section(tt_base,	0x00000000,				0x00000000,				4096,					MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	SDRAM_WB_VA_BASE,		SDRAM_WB_PA_BASE,	 	SDRAM_WB_SIZE>>20,		MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,CB));	//RAM WB
	mmu_map_section(tt_base,	SDRAM_WT_VA_BASE,		SDRAM_WT_PA_BASE,	 	SDRAM_WT_SIZE>>20,		MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,NCB));	//RAM WT

	mmu_map_section(tt_base,	SFR_VA_BASE,			SFR_PA_BASE,			SFR_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_CHECK,NCNB));
	mmu_map_section(tt_base,	OCSRAM_VA_BASE,			OCSRAM_PA_BASE,			1,						MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_CHECK,CB));

	mmu_map_section(tt_base,	GIC_VA_BASE,			GIC_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	0xFFF00000,				RAM_PA_LAST_1MB,		1,						MMU_L1_SEC(TEX1,AP_USER_RW,DOMAIN_NO_CHECK,CB));

#endif

#if 0
	mmu_map_section(tt_base,	0x00000000,				0x00000000,	4096,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));

	//mmu_map_section(tt_base,	RAM_PA_BASE,			RAM_PA_BASE,			RAM_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));

	mmu_map_section(tt_base,	KERNEL_VA_BASE,			RAM_PA_BASE,	 		RAM_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));	//RAM WB
	mmu_map_section(tt_base,	PAGE_FRAME_VA_BASE,		PAGE_FRAME_PA_BASE,	 	PAGE_FRAME_MEM_SIZE>>20,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));	//RAM WT
	mmu_map_section(tt_base,	SFR_VA_BASE,			SFR_PA_BASE,			SFR_SIZE>>20,			MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	OCSRAM_VA_BASE,			OCSRAM_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CB));

	mmu_map_section(tt_base,	MMU_TT_VA_BASE,			MMU_TT_PA_BASE,			MMU_TT_SIZE>>20,		MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));
	mmu_map_section(tt_base,	DMA_BUFFER_VA_BASE,		DMA_BUFFER_PA_BASE,		DMA_BUFFER_SIZE>>20,	MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,CNB));
	mmu_map_section(tt_base,	GIC_VA_BASE,			GIC_PA_BASE,			1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_NO_CHECK,NCNB));
	mmu_map_section(tt_base,	0xFFF00000,				RAM_PA_LAST_1MB,		1,						MMU_L1_SEC(TEX0,AP_USER_RW,DOMAIN_CHECK,CB));


	//for test...
	mmu_map_section(tt_base,	0x00000000,				0x00000000,				1,				MMU_L1_FAULT);
#endif
}


/*===============================================================================================*/

void*	stack_malloc(u32 size)
{
	//return kmalloc(size);
	return vmalloc(size);
}

void	stack_free(void *p)
{
	//kfree(p);
	vfree(p);
}


/*=========================================================================================*/

void* dma_mem_alloc(u32 size)
{
	u8 *p;

	GUI_MutexLock(mutex_dma,5000);
	p =x_heap_alloc(&heap_dma,size);
	GUI_MutexUnlock(mutex_dma);
	if(p==NULL)
	{

	}

	return p;
}

void dma_mem_free(void *p)
{
	GUI_MutexLock(mutex_dma,5000);
	x_heap_free(&heap_dma,p);
	GUI_MutexUnlock(mutex_dma);
}

/*=========================================================================================*/

void Board_MemInit(void)
{
	GUI_DMA_Init();
	GUI_VMEM_Init();
}

/*=========================================================================================*/

//u32	GetMemTotSize(void)
//{
//	u32 m1,m2;
//
//	kmem_heap_info(&m1,NULL,NULL);
//	HeapInfo(heap_wb,&m2,NULL,NULL);
//	return (m1+m2);
//}

//u32	GetMemCurSize(void)
//{
//	u32 m1,m2;
//
//	kmem_heap_info(NULL,&m1,NULL);
//	HeapInfo(heap_wb,NULL,&m2,NULL);
//	return (m1+m2);
//}

/**
  * @brief  创建一个VMEM内存堆
  * @note  使用vmalloc前必须调用本函数初始化内存堆句柄
  * @retval 无
  */
void GUI_VMEM_Init(void)
{
	mutex_vmem = GUI_MutexCreate();
	x_heap_init(&heap_vmem,
			   (void *)VMEM_VA_BASE,
			    VMEM_SIZE,
			    32);	 /* 创建一个内存堆 */
}

/**
  * @brief  创建一个DMA内存堆
  * @note  使用vmalloc前必须调用本函数初始化内存堆句柄
  * @retval 无
  */
void GUI_DMA_Init(void)
{
	mutex_dma = GUI_MutexCreate();
	x_heap_init(&heap_dma,
				(void *)DMA_BUFFER_VA_BASE,
				DMA_BUFFER_SIZE,
				64);	 /* 创建一个内存堆 */
}

/**
* @brief  从内存堆里申请空间
* @param  size 要申请的内存大小
* @retval 申请到的内存指针
*/
void* GUI_VMEM_Alloc(u32 size)
{
	u8 *p;

	GUI_MutexLock(mutex_vmem,5000);
	p =x_heap_alloc(&heap_vmem,size);
	GUI_MutexUnlock(mutex_vmem);
  if(p==NULL)
	{

	}

	return p;

}

/**
* @brief  释放内存
* @param  p:需要释放的内存首址
* @retval 无
*/
void GUI_VMEM_Free(void *p)
{
	GUI_MutexLock(mutex_vmem,5000);
	x_heap_free(&heap_vmem,p);
	GUI_MutexUnlock(mutex_vmem);

}

/**
  * @brief  创建一个内存堆（用于GUI内核对象）,可参考vmem配置
  * @retval 无
  */
void	GUI_MEM_Init(void)
{
  /* 本示例中的GUI内核对象使用 pvPortMalloc ，
    它已由FreeRTOS系统初始化*/

	return ;
}

/*===================================================================================*/

/**
* @brief  显示动态内存申请(用于GUI显示器缓存)
* @param  size 要申请的内存大小
* @retval 申请到的内存指针
*/
void*	GUI_GRAM_Alloc(U32 size)
{
	return GUI_VMEM_Alloc(size);
}

/*===================================================================================*/

/**
* @brief  显示动态内存申请(用于GUI显示器缓存)
* @param  p:需要释放的内存首址
* @retval 无
*/
void	GUI_GRAM_Free(void *p)
{
	GUI_VMEM_Free(p);
}


void* vmalloc(u32 size)
{
	return GUI_VMEM_Alloc(size);
}

void vfree(void *p)
{
	GUI_VMEM_Free(p);
}

/********************************END OF FILE****************************/



/*=========================================================================================*/
