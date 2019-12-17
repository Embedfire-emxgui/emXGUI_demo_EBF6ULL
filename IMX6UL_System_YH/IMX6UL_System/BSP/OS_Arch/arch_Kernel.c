									 
#include "Include.h"
#include "x_libc.h"

/*===============================================================================================================*/

void SystemModuleInit(void)
{
	SYS_msleep(300);

	Board_ModuleInit();			//板件模块初始化
}


/*===============================================================================================================*/

void	HAL_FlushCacheAll(void)
{
	mmu_flush_cache_all();
	mmu_flush_tlb_all();
}

/*===============================================================================================================*/
#define	MTT_L1_CELL_SIZE	(16*KB)
#define	MTT_L2_CELL_SIZE	(1*KB)

#define	MTT_L1_VA_BASE		(MMU_TT_VA_BASE)
#define	MTT_L1_PA_BASE		(MMU_TT_PA_BASE)
#define	MTT_L1_NUM			(MMU_TT_SIZE/2/(MMU_L1_CELL_SIZE))

#define	MTT_L2_VA_BASE		(MMU_TT_VA_BASE+(MMU_TT_SIZE/2))
#define	MTT_L2_PA_BASE		(MMU_TT_PA_BASE+(MMU_TT_SIZE/2))
#define	MTT_L2_NUM			(MMU_TT_SIZE/2/MTT_L2_CELL_SIZE)


static list_mem L2_tbl[MTT_L2_NUM];
static list_mem L2_list;


////////////
#define	PAGE_FRAME_SIZE	(4*KB)
#define	PAGE_FRAME_NUM	(PAGE_FRAME_MEM_SIZE/PAGE_FRAME_SIZE)

static mem_page mem_page_tbl[PAGE_FRAME_NUM];
static mem_page mem_page_list;

/*=========================================================================================*/

static void	L2_mem_init(void)
{
	int i;
	u8 *addr;
	////

	L2_list.addr =NULL;
	L2_list.next =NULL;

	addr =(u8*)MTT_L2_VA_BASE;

	for(i=0;i<MTT_L2_NUM;i++)
	{
		L2_tbl[i].addr =addr;
		L2_tbl[i].next =L2_list.next;

		L2_list.next = &L2_tbl[i];

		addr += MTT_L2_CELL_SIZE;
	}
}

static list_mem*	L2_mem_alloc(void)
{
	list_mem *l2;
	int a;

	a =HAL_EnterCritical();
	l2 =L2_list.next;
	if(l2!=NULL)
	{
		L2_list.next =l2->next;
		l2->next =NULL;
	}
	HAL_ExitCritical(a);
	return l2;
}

static void	L2_mem_free(list_mem* l2)
{
	int a;

	if(l2!=NULL)
	{
		a =HAL_EnterCritical();
		l2->next =L2_list.next;
		L2_list.next =l2;
		HAL_ExitCritical(a);
	}
}

/*=========================================================================================*/

void	mem_page_init(void)
{
	int i;
	u8 *pa;
	////
	L2_mem_init();

//	mem_page_list.va =NULL;
	mem_page_list.pa =NULL;
	mem_page_list.next =NULL;

	pa =(u8*)PAGE_FRAME_PA_BASE;

	for(i=0;i<PAGE_FRAME_NUM;i++)
	{
		mem_page_tbl[i].pa =pa;
//		mem_page_tbl[i].va =NULL;

		mem_page_tbl[i].next =mem_page_list.next;

		mem_page_list.next = &mem_page_tbl[i];

		pa += PAGE_FRAME_SIZE;
	}
}

static mem_page* mem_page_alloc(void)
{
	mem_page *mem_page;
	int a;

	a =HAL_EnterCritical();
	mem_page =mem_page_list.next;

	if(mem_page!=NULL)
	{
		mem_page_list.next =mem_page->next;
		mem_page->next =NULL;
	}
	HAL_ExitCritical(a);
	return mem_page;
}

static void	mem_page_free(mem_page* mem_page)
{
	int a;

	if(mem_page!=NULL)
	{
		a =HAL_EnterCritical();
		mem_page->next =mem_page_list.next;
		mem_page_list.next =mem_page;
		HAL_ExitCritical(a);
	}
}

/*=========================================================================================*/


static BOOL	proc_mem_page_mmap(X_PROCESS *pcb,u8 *va,u32 size) //map 4KB.
{
	int i,L1_idx,L2_idx;
	u32 *L2_base,addr;
	list_mem *l2;
	mem_page *pf;
	////

	L1_idx =((u32)va>>20)&0x0FFF;
	L2_idx =((u32)va>>12)&0xFF;

	l2 =pcb->list_L2;
	pf =pcb->list_PF;
	while(size > 0)
	{
		L2_base =l2->addr;

		while(L2_idx < 256)
		{
			mmu_map_small_page(L2_base,L2_idx,(u32)pf->pa);

			L2_idx++;
			pf =pf->next;

			if(size > PAGE_FRAME_SIZE)
			{
				size -= PAGE_FRAME_SIZE;
			}
			else
			{//完成
				size = 0;
				break;
			}

		}

		addr =MTT_VA2PA((u32)L2_base); //TO L2_PA

		pcb->L1_base[L1_idx] = (u32)addr | MMU_L1_PAGE(DOMAIN_NO_CHECK);

		L1_idx++;
		L2_idx = 0;
		l2 =l2->next;

	}
	return TRUE;
}

static BOOL	proc_mem_L2_init(X_PROCESS *pcb,u32 size)
{
	list_mem *l2;

	while(size > 0)
	{
		l2 =L2_mem_alloc();
		if(l2==NULL)
		{
			//while(1);
			return FALSE;
		}

		//增加L2到进程控制块中
		l2->next =pcb->list_L2;
		pcb->list_L2 =l2;

		if(size > (1*MB))
		{
			size -= 1*MB;
		}
		else
		{
			size =0;
			break;
		}
	}
	return TRUE;
}

static BOOL	proc_mem_page_init(X_PROCESS *pcb,u32 size)
{
	mem_page *pf;

	while(size > 0)
	{
		pf=mem_page_alloc();
		if(pf==NULL)
		{
			//while(1);
			return FALSE;
		}
		//增加pf到进程控制块中
		pf->next =pcb->list_PF;
		pcb->list_PF=pf;

		if(size > PAGE_FRAME_SIZE)
		{
			size -= PAGE_FRAME_SIZE;
		}
		else
		{
			size =0;
			break;
		}
	}
	return TRUE;
}


BOOL	PROC_MemInit(X_PROCESS *pcb,int pid,u8 *va,u32 size)
{
	mtt_buf_t *mtt =(mtt_buf_t*)MMU_TT_VA_BASE;;


	pcb->PF_Size =PAGE_FRAME_SIZE;
	pcb->list_L2 =NULL;
	pcb->list_PF =NULL;
	pcb->L1_base =mtt[pid].L1;

	UserMemMap_Init(pcb ->L1_base);

	if(!proc_mem_L2_init(pcb,size))
	{
		goto err;
	}

	if(!proc_mem_page_init(pcb,size))
	{
		goto err;
	}

	proc_mem_page_mmap(pcb,va,size);
	return TRUE;

err:
	PROC_MemFree(pcb,pid);
	return FALSE;
}

void	PROC_MemFree(X_PROCESS *pcb,int pid)
{

	if(1)
	{
		mem_page *n,*next;

		n=pcb->list_PF;
		while(n!=NULL)
		{
			next =n->next;
			mem_page_free(n);
			n =next;
		}
		pcb->list_PF =NULL;
	}

	if(1)
	{
		list_mem *n,*next;

		n=pcb->list_L2;
		while(n!=NULL)
		{
			next =n->next;
			L2_mem_free(n);
			n=next;
		}
		pcb->list_L2 =NULL;
	}

	pcb->L1_base =NULL;
}

/*=========================================================================================*/

u8*	PageFrame_PA2VA(u8 *pa)
{
	return (u8*)PF_PA2VA(pa);
}

/*=========================================================================================*/

/*===============================================================================================================*/
/*===============================================================================================================*/
#if 0
	
static	char buf[200];
static	mtt_addr_t L1,L2;

void	mmu_test(void)
{

	char *p;
	U32 *def_mtt;
	int i;
	 

	DebugPuts("mmu_test0.\r\n");
  	
  	//DebugPutHex((U32)p,64,p);

	L1.va	=(U32*)MMU_TT_VA_Base;
	L1.pa	=(U32*)MMU_TT_PA_Base;

	
	L2.va	=(U32*)(MMU_PAGE_TBL_VA_Base+(2*1*KB));
	L2.pa	=(U32*)(MMU_PAGE_TBL_PA_Base+(2*1*KB));

	
	DebugPrintf("L1: %08XH,%08XH.\r\n",L1.va,L1.pa);
	DebugPrintf("L2: %08XH,%08XH.\r\n",L2.va,L2.pa);
		
	for(i=0;i<0x00FFFFFF;i++);
	 

	mmu_map_small_page(&L2,0x00,0x30000000);
	mmu_map_small_page(&L2,0x01,0x30001000);
	mmu_map_section_as_page(&L1,0xF00,&L2);
	
	/*
	mmu_remap_small_page(&L1,&L2,0xF0000000,0x30000000);
	mmu_remap_small_page(&L1,&L2,0xF0001000,0x30001000);
    */
   
   	 /*   	 	 
    	   
    p=(char*)(0xF0000000+((0*4)*KB));
    for(i=0;i<128;i++)
    {
    	*p++ =i;
    }  
    
    p=(char*)(0xF0000000+((1*4)*KB));
    for(i=128;i<256;i++)
    {
    	*p++ =i;
    } 
	 */
    p=(char*)(0xF0000000+((0*4)*KB));
    DebugPuts("mmu_test1.\r\n");
   	DebugPutHex((U32)p,64,p);
  
  	
  	p=(char*)(0xF0000000+((1*4)*KB));
    DebugPuts("mmu_test2.\r\n");
   	DebugPutHex((U32)p,64,p);
  	
  	DebugPuts("mmu_test3.\r\n");	
	   
}

#endif 
 
/*===============================================================================================================*/
/*===============================================================================================================*/

void	MTT_Switch(U32 new_pid,U32 old_pid)
{
	U32 *mtt_pa;
	int a;

		
	if(new_pid==old_pid)
	{
		return;
	}

	a=HAL_EnterCritical();
		
   	mmu_flush_cache_all();
	mmu_flush_tlb_all();
	
	mtt_pa =((mtt_buf_t*)MMU_TT_PA_BASE)[new_pid].L1;

	mmu_set_mtt_base((U32)mtt_pa);

	HAL_ExitCritical(a);
	
}


/*===============================================================================================================*/

int	HAL_SetNetConfig(const net_config_t *cfg)
{
	return	FALSE;//NetDev_SetConfig(cfg);
}

int	HAL_GetNetConfig(net_config_t *cfg)
{
	return	FALSE;//NetDev_GetConfig(cfg);
}


/*===============================================================================================================*/

void	SysDrv_GetProcessorInfo(PROCESSOR_INFO *info)
{
	if(info!=NULL)
	{
		x_wstrcpy((WCHAR*)info->Family,L"Cortex-A7");
		x_wstrcpy((WCHAR*)info->String,L"MCIMX6G");
		info->ID	=0x00000972;
		info->Clock	=528*1000000;//CPU_GetFclkHZ()/1000000;

	}
}

/*===============================================================================================================*/
