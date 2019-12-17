
/*
 *LCD_BL_Setup.c
 *2019年7月3日下午8:07:40
 *
 */

#include "emXGUI.h"
#include "BSP.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000
#define	ID_SCROLL1	0x1001

#define	WIN_W	GUI_XSIZE
#define	WIN_H	100

void BL_PWM_Set(u8 val);

/*============================================================================*/


#define	MOVE_STOP	0
#define	MOVE_UP		1
#define	MOVE_DOWN	2
#define	MOVE_EXIT	3

static int move_stat=0;
static u8 bl_val=30;
void BL_PWM_Set(u8 val);

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			move_stat =MOVE_DOWN;
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			//CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

			if(1)
			{

				HWND wnd;
				SCROLLINFO sif;
				sif.cbSize		=sizeof(sif);
				sif.fMask		=SIF_ALL;
				sif.nMin		=1;
				sif.nMax		=100;

				sif.nValue		=bl_val;
				sif.TrackSize		=60;
				sif.ArrowSize		=0;//20;//20;

				wnd = CreateWindow(SCROLLBAR,L"HScroll",SBS_NOARROWS|WS_VISIBLE,0,0,rc.w,50,hwnd,ID_SCROLL1,NULL,NULL);
				SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
			}

			SetTimer(hwnd,1,50,TMR_START,NULL);
		}
		return TRUE;
		////

		case WM_TIMER:
		{
			if(move_stat==MOVE_DOWN)
			{
				int endy=20;
				GetWindowRect(hwnd,&rc);
				if(rc.y < endy)
				{
					rc.y += MAX(1,(endy-rc.y)>>2);
					rc.y  =MIN(rc.y,endy);

					ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
					MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
				else
				{
					move_stat =MOVE_STOP;
				}

			}

			if(move_stat==MOVE_UP)
			{
				int endy =-WIN_H;
				GetWindowRect(hwnd,&rc);
				if(rc.y >  endy)
				{
					rc.y -= MAX(1,(rc.y-endy)>>2);
					rc.y  =MAX(rc.y,endy);

					ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
					MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
				else
				{
					move_stat =MOVE_EXIT;
					PostCloseMessage(hwnd);
				}

			}

		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}

			if(id == ID_SCROLL1)
			{
				NM_SCROLLBAR *nr;
				int i;

				nr =(NM_SCROLLBAR*)lParam;
				if(code==SBN_THUMBTRACK) //滑块移动
				{
					bl_val =nr->nTrackValue;
					BL_PWM_Set(bl_val);
					SendMessage(nr->hdr.hwndFrom,SBM_SETVALUE,TRUE,bl_val); //设置位置值


				}

			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			if(move_stat==MOVE_EXIT)
			{
				return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
			}
			else
			{
				move_stat=MOVE_UP;
			}
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

//static void	AppMain(void)
void	LCD_BL_Setup(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	int x,y,w,h;
	/////

	w =WIN_W-8*2;
	h =WIN_H;
	x =(GUI_XSIZE-w)/2;
	y =-(h+4);

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								L"Backlight", //窗口名称
								WS_OVERLAPPEDWINDOW,
								x,y,w,h,    //窗口位置和大小
								NULL,0,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/

#if 1
static int app_run=FALSE;
static int app_thread(void *argv)
{
	LCD_BL_Setup();
	app_run=FALSE;
}

void	app_LCD_BL_Setup(HWND hwndPrev)
{
	if(app_run==FALSE)
	{
		app_run =TRUE;
		GUI_Thread_Create(app_thread,hwndPrev,4096,NULL,0);
	}
	else
	{
#if 0
		MSGBOX_OPTIONS ops;
		const WCHAR *btn[]={L"Return"};

		ops.Flag =MB_BTN_WIDTH(60)|MB_ICONINFORMATION;
		ops.pButtonText =btn;
		ops.ButtonCount =1;

		MessageBox(hwndPrev,20,50,250,150,L"程序正在运行 ...",L"信息",&ops);
#endif
	}

}
#endif
/*=========================================================================*/

