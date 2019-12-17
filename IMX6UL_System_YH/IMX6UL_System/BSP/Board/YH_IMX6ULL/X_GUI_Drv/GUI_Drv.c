
#include	"Include.h"
#include	"GUI_Drv.h"
#include    "GUI_Drv_Cfg.h"

#include    "x_file.h"

/*===================================================================================*/


void LCD_HardInit(u32 lcd_fb_addr);
void LCD_BkLight(int on);


/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*
函数功能: GUI低级别的初始化
参数: 无
返回: TRUE:成功; FALSE：失败.
说明: 这是GUI的第一个初始化函数.
*/
BOOL GUI_LowLevelInit(void)
{
	Board_MemInit();

	return TRUE;
}

/*===================================================================================*/
/*===================================================================================*/
/*
函数功能: GUI显示设备的初始化
参数: 无
返回: 显示设备Surface对象指针
说明:
*/
SURFACE* GUI_DisplayInit(void)
{

	int width_bytes;
	u8 *lcd_buffer = NULL;
	SURFACE *pSurf;
	////

	//DebugPuts("GUI_DisplayInit\r\n");

#if	G2D_EN
	PXP_DrvInit();
#endif

#if	(LCD_FORMAT == COLOR_FORMAT_RGB565)

	//初始化LCD Surface结构数据(RGB565格式)
	lcd_buffer =(u8*)dma_mem_alloc(LCD_XSIZE*LCD_YSIZE*2);
	if (lcd_buffer == NULL)
	{
		while(1);
	}
	pSurf =CreateSurface(SURF_RGB565,LCD_XSIZE,LCD_YSIZE,LCD_XSIZE*2,lcd_buffer);

#endif

#if	(LCD_FORMAT == COLOR_FORMAT_XRGB8888)
	//初始化LCD Surface结构数据(XRGB8888格式)
	lcd_buffer =(u8*)dma_mem_alloc(LCD_XSIZE,LCD_YSIZE*4);
	pSurf =CreateSurface(SURF_XRGB8888,LCD_XSIZE,LCD_YSIZE,LCD_XSIZE*4,lcd_buffer);

#endif



	pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,0));
	LCD_HardInit((u32)DMA_MEM_VA2PA(lcd_buffer)); //LCD硬件初始化

#if FRAMEBUFFER_EN //使用帧缓冲支持(帧缓冲使用屏幕相同的格式和尺寸).
	{
		const SURFACE  *pSurf_FB;

		pSurf_FB =CreateSurface(pSurf->Format,pSurf->Width,pSurf->Height,0,NULL);
		GUI_SetFrameBufferSurface(pSurf_FB);
	}
#endif

	pSurf->GL->FillArea(pSurf,0,0,LCD_XSIZE,LCD_YSIZE,pSurf->CC->MapRGB(0x00,0x00,0x00)); //清屏
	LCD_BkLight(TRUE); //打开背光
/*
//	rIOMUXC_MUX_LCD_DATA04 =5;
//	rIOMUXC_CFG_LCD_DATA04 =0x10B0;

	gpio_pin_config_t cfg;

	cfg.direction =kGPIO_DigitalOutput;
	cfg.interruptMode =kGPIO_NoIntmode;
	cfg.outputLogic =0;
	GPIO_PinInit(GPIO3,9,&cfg);
*/
	while(0)
	{ //测试
/*
		GPIO_PinWrite(GPIO3,9,1);
		GUI_msleep(100);
		GPIO_PinWrite(GPIO3,9,0);
		GUI_msleep(100);
*/
#if 1
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,0,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,0,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,255,0));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,255));
		GUI_msleep(200);
		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,0));
		GUI_msleep(200);
		break;
#endif
	}

	return pSurf;
}


/*===================================================================================*/
/*
函数功能: GUI输入设备的初始化
参数: 无
返回: 无
说明:
*/
void GUI_InputInit(void)
{
#if(GUI_KEYBOARD_EN)
{
	KeyBoardDev_Init();
}
#endif

#if(GUI_TOUCHSCREEN_EN)
{
	if(TouchDev_Init()) /*触摸屏设备初始化*/
	{
#if(GUI_TOUCHSCREEN_CALIBRATE)
		TS_CFG_DATA ts_cfg;
		if(TouchDev_LoadCfg(&ts_cfg)) /*加载校正数据(电阻屏需要)*/
		{
			TouchPanel_TranslateInit(&ts_cfg); /*初始化坐标转换函数(电阻屏需要)*/
		}
#endif
	}

}
#endif

#if(GUI_MOUSE_EN)
{
	MouseDev_Init();
}
#endif

}

/*===================================================================================*/
/*
函数功能: GUI输入设备的执行
参数: 无
返回: 无
说明: 该函数会被周期性轮询调用
*/
void GUI_InputHandler(void)
{

#if(GUI_KEYBOARD_EN)
{
	if(KeyBoardDev_IsActive())
	{
		KeyBoardDev_Handler();
	}
}
#endif

#if(GUI_TOUCHSCREEN_EN)
{
	GUI_TouchHandler(); //调用触摸屏处理函数
}
#endif

#if(GUI_MOUSE_EN)
{
	if(MouseDev_IsActive())
	{
		MouseDev_Handler();
	}
}
#endif

}

/* FreeRTOS 系统头文件 */
  #include <stdio.h>
  #include "FreeRTOS.h"
  #include "task.h"

  /* 系统管理的内存堆，gui_mem_port.c */
  #define OS_MALLOC   pvPortMalloc
  #define OS_FREE     vPortFree

/*===================================================================================*/
/*
函数功能: 动态内存申请(用于GUI内核对象)
参数: size: 需要申请内存的字节数
返回: 内存首址
说明:
*/

void*	GUI_MEM_Alloc(U32 size)
{
	void *p=NULL;

	p =OS_MALLOC(size);
	if(p==NULL)
	{
	    GUI_Printf("#Error: GUI_MEM_Alloc.\r\n");
	}

	return p;
}

/*===================================================================================*/
/*
函数功能: 动态内存释放(用于GUI内核对象)
参数: p:需要释放的内存首址
返回: 无
说明:
*/

void	GUI_MEM_Free(void *p)
{
	OS_FREE(p);
}

/*===================================================================================*/

/*
函数功能: 格式化字符串输出
参数: 可变参数
返回: 无
说明: 如果不用输出GUI调试信息,这个函数可以为空
*/

void	GUI_Printf(const char *fmt,...)
{
#if 0
  	x_va_list ap;
    char *buf;
    static	char str_buf[256];
    static  GUI_MUTEX *mutex_gui_printf=NULL;
  	////
  	buf	=str_buf/*(char*)GUI_MEM_Alloc(256)*/;
  	if(buf)
  	{

    	if(GUI_MutexLock(mutex_gui_printf,0xFFFFFFFF))
    	{
    	    x_va_start(ap,fmt);
       	 	x_vsprintf(buf,fmt,ap);
       		DebugPuts(buf);
       		//DebugOutput(buf);

       		GUI_MutexUnlock(mutex_gui_printf);
       		x_va_end(ap);
   		}

		//GUI_MEM_Free(buf);
	}
#endif
}

/*===================================================================================*/
/*===================================================================================*/

#if FONT_XFT_EN

//#include <GUI_Font_XFT.h>

extern const char GB2312_16_2BPP[];
extern const char GB2312_16_4BPP[];
extern const char GB2312_20_2BPP[];
extern const char GB2312_24_2BPP[];
extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];

static const char font_path_xft[] ="B:kernel/fonts/GB2312_20_4BPP.xft";
static u8 *pFontData_XFT=NULL;

#endif

//////////////


#if FONT_TTF_EN

#include    "FontDrv_FreeType.h"

struct __freetype_font_obj *ft_obj;


#endif


/*===================================================================================*/

#if 0

int RES_GetOffset(const char *res_name);
int RES_Read(u8 *buf,u32 addr,int size);

static int read_data(void *buf,int offset,int size,LONG lParam)
{
	offset += lParam;
	RES_Read(buf,offset,size);
	return size;
}

#endif

/*===================================================================================*/
/*
函数功能: GUI默认字体初始化
参数: 无
返回: 返回默认字体的句柄
说明:
*/

HFONT GUI_FontInit(void)
{

	HFONT hFont=NULL;

#if	FONT_XFT_EN  /*XFT格式字体*/
{

#if 0
{
	/*丛文件系统加载本地内存*/

    X_FILE *fp;
    int fsize;

    fp =x_fopen(font_path_xft,"rb");
    if(fp != NULL)
    {
    	fsize =x_fsize(fp);

    	pFontData_XFT =(u8*)vmalloc(fsize);
    	if(pFontData_XFT!=NULL)
    	{
    		x_fread((u8*)pFontData_XFT,fsize,1,fp);
    	}
    	x_fclose(fp);
    	////
    	hFont 	=XFT_CreateFont(pFontData_XFT);
    }
}
#endif

#if 0
    if(hFont==NULL)
    { /*丛流设备加载(RES储存设备)*/

    	int offset;
    	offset =RES_GetOffset("GB2312_20_4BPP.xft");
    	if(offset > 0)
    	{
    		hFont =XFT_CreateFontEx(read_data,offset);
    	}
    }
#endif

    if(hFont==NULL)
    { /*丛本地加载(本地数组数据)*/

    	//hFont =XFT_CreateFont(ASCII_16_4BPP);  /*ASCii字库,16x16,4BPP抗锯齿*/

    	hFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312字库,16x16,2BPP抗锯齿*/
    	//hFont =XFT_CreateFont(ASCII_20_4BPP);  /*ASCii字库,20x20,4BPP抗锯齿*/
    	//hFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312字库,20x20,4BPP抗锯齿*/
    }

}
#endif

#if	FONT_TTF_EN /*TTF格式字体*/
{

    X_FILE *fp;
    u8 *pFontBuffer=NULL;
    const char *font_path ="C:kernel/fonts/def_font.ttf";
    int fsize;

    fp =x_fopen(font_path,"rb");
    if(fp != NULL)
    {
    	fsize =x_fsize(fp);
    	pFontBuffer =(u8*)vmalloc(fsize);
    	{
    		x_fread((u8*)pFontBuffer,fsize,1,fp);
    	}
    	x_fclose(fp);
    	////
    }

    ft_obj=freetype_font_obj_new(20,20,72,72,pFontBuffer);
    hFont =freetype_CreateFont(ft_obj);

}
#endif

	return hFont;
}


/*===================================================================================*/
void	GUI_UserAppStart(void);
//void    ShellWindowExecu(void);
void	app_GUI_DEMO(HWND hwndPrev);

static	int	gui_app_thread(void *p)
{

    #if(GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
    {
        int i=0;
      //  while(TouchPanel_IsPenDown())
    	{
    		GUI_msleep(100);
    		//if(i++>10)
    		{
    			ShowCursor(FALSE);
    			TouchScreenCalibrate();
    			ShowCursor(TRUE);
    			//break;
    		}
    	}
    }
    #endif
    while(1)
    {
    	app_GUI_DEMO(NULL);
    	GUI_msleep(100);
    }
    //GUI_UserAppStart();
  //  ShellWindowExecu();
    return 0;
}

/*===================================================================================*/

static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
{
	RECT rc;

	if(lprc==NULL)
	{
		GetClientRect(hwnd,&rc);
	}
	else
	{
		CopyRect(&rc,lprc);
	}

	SetBrushColor(hdc,MapRGB(hdc,32,72,144));
	FillRect(hdc,&rc);

	SetTextColor(hdc,MapRGB(hdc,250,250,250));
	TextOut(hdc,20,30,L"桌面窗口",-1);
#if 0
	if(1)
	{
		WCHAR wbuf[128];
		U32 UPLLclk,APLLclk,SYSclk,Pclk,CPUclk;
		U32 Hclk1,Hclk234;

		UPLLclk =sysGetClock(SYS_UPLL);
		APLLclk =sysGetClock(SYS_APLL);
		SYSclk =sysGetClock(SYS_SYSTEM);
		CPUclk =sysGetClock(SYS_CPU);
		Pclk =sysGetClock(SYS_PCLK);

		Hclk1 =sysGetClock(SYS_HCLK1);
		Hclk234 =sysGetClock(SYS_HCLK234);

		x_wsprintf(wbuf,L"UPLL:%d,APLL:%d,SYS:%d,CPU:%d,PCLK:%d,Hclk1:%d,Hclk234:%d"
				,UPLLclk,APLLclk,SYSclk,CPUclk,Pclk,Hclk1,Hclk234);


		TextOut(hdc,20,60,wbuf,-1);
	}
#endif
}

static GUI_SEM* sem_input=NULL;

void	GUI_InputPost(void)
{
	if(sem_input==NULL)
	{
		return;
	}
	GUI_SemPost(sem_input);

}

static	int	gui_input_thread(void *p)
{
	static int i=0;
	GUI_msleep(100);
	while(1)
	{
		GUI_SemWait(sem_input,1000);
		//WDT_Clr();
		i++;
		if(i&1)
		{
			GUI_InputHandler(); //处理输入设备
		}
		//GUI_msleep(50);
	}
}

//用户的桌面回调函数.
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    RECT rc;

	switch(msg)
	{
		case	WM_CREATE:	//桌面创建时,会产生该消息,可以在这里做一些初始化工作.

				sem_input =GUI_SemCreate(0,1);

		        //初始化输入设备
		        GUI_InputInit();

				////创建1个30ms定时器，处理循环事件.
				SetTimer(hwnd,1,30,TMR_START,NULL);

		        //GUI_Thread_Create(gui_input_thread,NULL,8*KB,NULL,0);

				//创建App线程
		        GUI_Thread_Create(gui_app_thread,NULL,32*KB,NULL,0);

				break;
				////

		case	WM_LBUTTONUP:
		{
#if 0
			static int a=ROTATE_0;

			POINT pt;

			pt.x =LOWORD(lParam);
			pt.y =HIWORD(lParam);

			rc.x =0;
			rc.y =0;
			rc.w =20;
			rc.h =20;

			if(PtInRect(&rc,&pt))
			{
				a=disp_rotate;
				if(++a > ROTATE_270) a=ROTATE_0;

				gdrvSetRotate(a);

				MoveWindow(hwnd,0,0,surf_screen.Width,surf_screen.Height,TRUE);

				GetWindowRect(hwnd,&rc);
				ClipCursor(&rc);

				RedrawWindow(hwnd,NULL,NULL,RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE|RDW_FRAME);

			}

#endif
			return	DefDesktopProc(hwnd,msg,wParam,lParam);
		}

		////////
		case	WM_TIMER:
		{
			u16 id;

			id =LOWORD(wParam);
			if(id==1)
			{
				GUI_InputHandler(); //处理输入设备
			}
		}
		break;
		////
		case WM_ERASEBKGND:
		{

			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB(hdc,50,80,150));
			FillRect(hdc,&rc);
		}
		return TRUE;
		////
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;

			//GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			rc.x =10;
			rc.y =20;
			rc.w =150;
			rc.h =28;
			SetBrushColor(hdc,MapRGB(hdc,40,60,100));
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			DrawText(hdc,L"emXGUI Desktop",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

void GUI_DesktopStartup(void)
{
	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= desktop_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
								&wcex,
								L"DESKTOP",
								WS_VISIBLE|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,
								NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

	//设置系统打开光标显示(可以按实际情况看是否需要).
	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}
