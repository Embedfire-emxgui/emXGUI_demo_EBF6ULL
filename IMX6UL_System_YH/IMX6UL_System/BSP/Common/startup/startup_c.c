

#include "def.h"
/*=========================================================================================*/


extern const u8 __vector_base[];
extern const u8 __vector_end[];

extern const struct __copy_table __copy_tabel_start__;
extern const struct __copy_table __copy_tabel_end__;

extern const struct __clr_table __clr_tabel_start__;
extern const struct __clr_table __clr_tabel_end__;

/*=========================================================================================*/

struct __copy_table
{
	u32 *dst,*src,size;
};

struct __clr_table
{
	u32 *dst,size;
};

/*=========================================================================================*/

static void __memcpy32(u32 *dst,u32 *src,s32 size)
{

	while(size > 0)
	{
		*dst++ = *src++;
		 size -= 4;
	}
}

static void __memset32(u32 *dst,s32 size)
{

	while(size > 0 )
	{
		*dst++ = 0x00000000;
		size -= 4;
	}
}

void CStartup(void)
{

	if(1)
	{
		struct __copy_table *copy;

		copy =(struct __copy_table*)&__copy_tabel_start__;
		while((u32)copy <(u32)&__copy_tabel_end__)
		{
			__memcpy32(copy->dst,copy->src,copy->size);
			copy++;
		}
	}

	if(1)
	{
		struct __clr_table *clr;

		clr =(struct __clr_table*)&__clr_tabel_start__;
		while((u32)clr < (u32)&__clr_tabel_end__)
		{
			__memset32(clr->dst,clr->size);
			clr++;
		}
	}

	if(1)
	{
		__memcpy32((u32*)0xFFFF0000,(u32*)__vector_base,(u32)__vector_end-(u32)__vector_base);
	}

	HAL_FlushCacheAll();
	system_init();
	Board_Startup();
}

/*=========================================================================================*/
