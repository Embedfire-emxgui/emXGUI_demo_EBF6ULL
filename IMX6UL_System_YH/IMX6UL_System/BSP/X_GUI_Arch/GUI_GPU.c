
#include "GUI_Drv.h"
#include "BSP.h"

/*===================================================================================*/
/*
 *GPU绘图加速驱动函数,如果不支持操作可以直接返回FALSE.
 */

#if(G2D_EN)

#include	"Common/PXP/pxp_drv.h"

#define	GPU_EN	1

#endif

/*===================================================================================*/

#if 0
BOOL	GUI_GPU_Init(void)
{

	return TRUE;
}
#endif

/*============================================================================*/
/*
 * 将指定区域原始像素复制到out缓冲区.
 */
BOOL 	GPU_CopyBits(const SURFACE *pSurf,int x,int y,int w,int h,void *out,int width_bytes)
{
	return FALSE;
}

/*============================================================================*/
/*
 * 矩形填充.
 */
BOOL	GPU_FillRect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
#if(GPU_EN)
	{
		if(PXP_fill_rect(pSurf,x,y,w,h,c))
		{
			return TRUE;
		}
		return FALSE;
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/
/*
 * 矩形填充(ARGB方式).
 */
BOOL	GPU_FillRectARGB(const SURFACE *pSurf,int x,int y,int w,int h,U8 a,U8 r,U8 g,U8 b)
{
	return FALSE;
}

/*============================================================================*/
/*
 * 绘制位图.
 */
BOOL	GPU_DrawBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const U8 *bits,int width_bytes,int format)
{
#if(GPU_EN)
	{
		switch(format)
		{

		case BM_RGB565:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB565);

		case BM_XRGB1555:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB555);

		case BM_ARGB4444:
			return	PXP_draw_bitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_AsPixelFormatARGB4444);

		case BM_RGB888:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB888);

		case BM_XRGB8888:
			return	PXP_draw_bitmap_RGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_PsPixelFormatRGB888);

		case BM_ARGB8888:
			return	PXP_draw_bitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,kPXP_AsPixelFormatARGB8888);

		default:
			return FALSE;

		}
	}
#else
	return FALSE;
#endif

}

/*============================================================================*/
/*
 * 缩放位图.
 */
BOOL 	GPU_ScaleBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
#if(GPU_EN)
	{
		return	PXP_ScaleBitmap(pSurf,x,y,w,h,bm);
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/


