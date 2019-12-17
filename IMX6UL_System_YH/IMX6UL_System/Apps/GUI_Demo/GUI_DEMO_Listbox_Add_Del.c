
#include "emXGUI.h"
#include "x_libc.h"


/*===================================================================================*/
#define	ID_EXIT		0x1000
#define	ID_ADD		0x1001
#define	ID_DEL		0x1002
#define	ID_LISTBOX1	0x1101
#define	ID_LISTBOX2	0x1102
#define	ID_LISTBOX3	0x1103

/*===================================================================================*/


static int idx=0;

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc,rc0;
	WCHAR wbuf[128];
	int i,x,y;
	HWND wnd;
	
	switch(msg)
	{
		case	WM_CREATE:
				
				idx=0;
				GetClientRect(hwnd,&rc0);

				rc.w =70;
				rc.h =30;
				rc.x =rc0.w-rc.w-4;
				rc.y =4;
				CreateWindow(BUTTON,L"Exit",WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL);
				
				OffsetRect(&rc,0,rc.h+8);
				CreateWindow(BUTTON,L"ADD",WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_ADD,NULL,NULL);
				OffsetRect(&rc,0,rc.h+8);
				CreateWindow(BUTTON,L"DEL",WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_DEL,NULL,NULL);

				wnd=CreateWindow(LISTBOX,L"Listbox",LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,8,160,200,hwnd,ID_LISTBOX1,NULL,NULL);

				return TRUE;
				////////
		

		case	WM_NOTIFY:
		{
			u16 code,id;

			code =HIWORD(wParam);
			id   =LOWORD(wParam);

			if(code==BN_CLICKED && id == ID_ADD)
			{
				x_wsprintf(wbuf,L"Item-%03d",idx++);
				wnd =GetDlgItem(hwnd,ID_LISTBOX1);
				SendMessage(wnd,LB_ADDSTRING,0xFFFF,(LPARAM)wbuf);
				InvalidateRect(wnd,NULL,TRUE);
			}

			if(code==BN_CLICKED && id == ID_DEL)
			{
				int cur;
				wnd =GetDlgItem(hwnd,ID_LISTBOX1);

				cur =SendMessage(wnd,LB_GETCURSEL,0,0);
				SendMessage(wnd,LB_DELETESTRING,cur,0);
				InvalidateRect(wnd,NULL,TRUE);
			}

		}
		break;
		////


		case	WM_PAINT:
				hdc	=BeginPaint(hwnd,&ps);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
					
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Listbox_Add_Del(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
	
		//(��һ����.)
		InvalidateRect(GetDesktopWindow(),NULL,TRUE);
		
		/////
		wcex.Tag 		    = WNDCLASS_TAG;

		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		
		
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									L"GUI Demo - Listbox:ADD & DEL",
									WS_OVERLAPPEDWINDOW,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);
		
			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
