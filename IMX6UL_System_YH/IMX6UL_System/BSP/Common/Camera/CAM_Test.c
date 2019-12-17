
#include	"Include.h"

#include "bsp_csi.h"
//#include "libsdk_jpg.h"
/*============================================================================*/

void CAM_Test(void)
{
	volatile int fps,frame,t1,t2;

	BITMAP bm;
	HDC hdc;
	HWND hwnd;

	CAM_DrvInit();
	////
	frame =0;
	fps   =0;
	t1 =GUI_GetTickCount();
	while(1)
	{

		if(CAM_SyncOut(&bm,1000))
		{
			frame++;
			t2 =GUI_GetTickCount();
			if((t2-t1) >= 1000)
			{
				fps =frame;
				frame =0;
				t1 =t2;
			}

			hwnd =GetDesktopWindow();
			hdc =GetDC(hwnd);
			DrawBitmap(hdc,10,20,&bm,NULL);
			ReleaseDC(hwnd,hdc);
			CAM_Continue();
		}
	}

	CAM_DrvDeinit();
}

/*============================================================================*/
#if 0
void CAM_Test_JPG(void)
{
	volatile int fps,frame,t1,t2,jpg_enc_time,jpg_dec_time,jpg_a,jpg_size_max;

	BITMAP bm;
	HDC hdc,hdc_mem;
	HWND hwnd;

	SURFACE *pSurf;
	u8 *vbuf;
	u8 *jpg_buf;
	u32 jpg_buf_size;
	u32 jpg_size;

	u32 vbuf_w,vbuf_h;
	WCHAR wbuf[200];
	local_time_t tm;

	CAM_DrvInit();

	////
	jpg_a =50;
	vbuf_w =320;
	vbuf_h =400;
	jpg_enc_time =0;
	jpg_dec_time =0;

	jpg_buf_size =512*1024;
	jpg_buf =GUI_GRAM_Alloc(jpg_buf_size);

	vbuf =GUI_GRAM_Alloc(vbuf_w*vbuf_h*4);

	pSurf =CreateSurface(SURF_XRGB8888,vbuf_w,vbuf_h,vbuf_w*4,vbuf);
	hdc_mem =CreateDC(pSurf,NULL);

	hwnd =GetDesktopWindow();

	bm.Format     =BM_RGB565;
	bm.Width      =APP_CAMERA_WIDTH;
	bm.Height     =APP_CAMERA_HEIGHT;
	bm.WidthBytes =APP_CAMERA_WIDTH*2;
	bm.LUT =NULL;

	jpg_size_max =0;
	frame=0;
	fps =0;
	t1 =GUI_GetTickCount();
	while(1)
	{
		CAM_SyncOut(&bm,5000);

		 frame++;
		 t2 =GUI_GetTickCount();
		 if((t2-t1) >= 1000)
		 {
			 fps =frame;
			 frame =0;
			 t1 =t2;
		 }

		 ScaleBitmap(hdc_mem,0,0,vbuf_w,vbuf_h,&bm);
		 //DrawBitmap(hdc_mem,0,0,&bm,NULL);

		 SYS_get_local_time(&tm);
		 SetTextColor(hdc_mem,MapRGB(hdc_mem,250,10,10));
		 x_wsprintf(wbuf,L"fps:%d,w:%d,h%d\r\n"
				 	 	 L"jpg_fsize:%d / %d\r\n"
				 	 	 L"enc_time:%dms,dec_time:%dms\r\n"
						 L"%04d/%02d/%02d  %02d:%02d:%02d\r\n"

				 ,fps,vbuf_w,vbuf_h
				 ,jpg_size_max,jpg_size
				 ,jpg_enc_time,jpg_dec_time
				 ,tm.year,tm.mon,tm.day,tm.hour,tm.min,tm.sec

				 );
		 TextOut(hdc_mem,4,8,wbuf,-1);

		 jpg_enc_time =GUI_GetTickCount();
		 jpg_size =JPEG_Encode_XRGB8888(jpg_buf,jpg_buf_size,vbuf,vbuf_w,vbuf_h,vbuf_w*4,jpg_a);
		 jpg_enc_time =GUI_GetTickCount()-jpg_enc_time;

		 if(jpg_size > jpg_size_max)
		 {
			 jpg_size_max =jpg_size;
		 }

		 hdc =GetDC(hwnd);
		//DrawBitmap(hdc,0,0,&bm,NULL);

		 //BitBlt(hdc,10,20,320,240,hdc_mem,0,0,SRCCOPY);

		 if(jpg_size > 0)
		 {
			 u16 w,h;
			 jpg_dec_time =GUI_GetTickCount();
			 if(JPEG_Info(&w,&h,jpg_buf))
			 {
				JPEG_Out(hdc,10,20,jpg_buf,jpg_size);
			 }
			 jpg_dec_time =GUI_GetTickCount()-jpg_dec_time;
		 }
		 ReleaseDC(hwnd,hdc);

		 CAM_Continue();
	}

	DeleteDC(hdc_mem);
	DeleteSurface(pSurf);

	CAM_DrvDeinit();

}
#endif

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
