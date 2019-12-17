#include	"Include.h"
#include	"x_file.h"
#include	"AutoRun.h"


/*============================================================================*/
static	HANDLE hInst=NULL;

/*============================================================================*/
static	struct	scan_dir_data s_data;
static	char	scan_file_path[1024];
static	char	scan_file_name_buf[256];
/*============================================================================*/
   
static	eFILESCAN_CMD	scan_file_callback(const char *path,const char *name,struct dir_info *d_info,const void* pdata)
{
	HWND hListWnd;
	 
	if(d_info->attr&FA_DIR)
	{
		return	SCAN_DIR;
	}
	 
	hListWnd	=(HWND)pdata;
	if(1)
	{
		WCHAR *wbuf;
		CHAR *buf;
		char *fn;
		int i,j;
		
		fn	=(char*)name;
		j =x_strlen(fn);
		while(j>0)
		{
			if(fn[j]=='.')
			{
				if(x_strncasecmp(".app",&fn[j],4)==0)	//检查搜索到的文件,扩展名是否为 .app 
				{
					
					i =x_strlen(path) + x_strlen(name) + 8;
					
					buf  =(CHAR*)vmalloc(i);
					wbuf =(WCHAR*)vmalloc(i*2);	//unicode 字符每个为2字节.
					
					x_sprintf(buf,"%s/%s",path,name);	//格式化成完整的目录字符串
					x_mbstowcs_cp936(wbuf,buf,i*2);		//将Ansi字符转换成GUI的unicode字符.
					 
					SendMessage(hListWnd,LB_ADDSTRING,SendMessage(hListWnd,LB_GETCOUNT,0,0),(LPARAM)wbuf);
					vfree(wbuf);
					vfree(buf);
				}
				break;
			}
			j--;
		}
	
	}
	  
	return	SCAN_NEXT;
	 
}


/*============================================================================*/
#define	ID_CANCEL	0x1001
#define	ID_CLR		0x1002
#define	ID_OK		0x1003

#define	ID_LIST		0x2001

static	int	Result=ID_CANCEL;
static	WCHAR sel_text[256];

static	int vk_return=FALSE;


static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int x,y;
	RECT rc,m_rc[4];
	
	
	switch(msg)
	{
		case	WM_CREATE:
		
				Result=ID_CANCEL;
				x=0;
				y=0;
				GetClientRect(hwnd,&rc);
				CreateWindow(LISTBOX,L"list",WS_VISIBLE,x,y,rc.w-x,rc.h-32,hwnd,ID_LIST,hInst,0);

				////
				s_data.fname_buf		=scan_file_name_buf;
				s_data.fname_buf_bytes	=256;
				s_data.path_buf			=scan_file_path;
				s_data.path_buf_bytes	=1024;
				s_data.callback			=scan_file_callback;
				s_data.pdata			=(void*)GetDlgItem(hwnd,ID_LIST);

				////从B:apps目录下查找
				x_strcpy(s_data.path_buf,"B:/apps");
				x_file_scan(&s_data);
				 
				////从C:apps目录下查找
				x_strcpy(s_data.path_buf,"C:/apps");
				x_file_scan(&s_data);
				 
				rc.x =0;
				rc.y =rc.h-32;
			//	rc.w =rc.w;
				rc.h =32;
				MakeMatrixRect(m_rc,&rc,4,4,3,1);
				
				CreateWindow(BUTTON,L"Cancel",WS_BORDER|WS_VISIBLE,m_rc[0].x,m_rc[0].y,m_rc[0].w,m_rc[0].h,hwnd,ID_CANCEL,hInst,0);
				CreateWindow(BUTTON,L"Clear",WS_BORDER|WS_VISIBLE,m_rc[1].x,m_rc[1].y,m_rc[1].w,m_rc[1].h,hwnd,ID_CLR,hInst,0);
				CreateWindow(BUTTON,L"OK",WS_BORDER|WS_VISIBLE,m_rc[2].x,m_rc[2].y,m_rc[2].w,m_rc[2].h,hwnd,ID_OK,hInst,0);
				break;
				////
		
		case	WM_KEYUP:
				{
					int vkey,i;
					HWND hwnd_list;
				
					hwnd_list =GetDlgItem(hwnd,ID_LIST);
					
					vkey =wParam;
					
					switch(vkey)
					{
						case	VK_UP:	
								i=SendMessage(hwnd_list,LB_GETCURSEL,0,0);
								i--;
								SendMessage(hwnd_list,LB_SETCURSEL,i,0);
								SendMessage(hwnd_list,LB_SETTOPINDEX,i-4,0);
								break;
								////
								
						case	VK_DOWN:
								i=SendMessage(hwnd_list,LB_GETCURSEL,0,0);
								i++;
								SendMessage(hwnd_list,LB_SETCURSEL,i,0);
								SendMessage(hwnd_list,LB_SETTOPINDEX,i-4,0);
								break;
								////
								
						case	VK_ESCAPE:		
						case	VK_LEFT:
								{
									//PostMessage(hwnd,WM_COMMAND,ID_CANCEL,0);
								}
								break;
								
						case	VK_RETURN:
								{	
									if(vk_return==TRUE)
									{
										//PostMessage(hwnd,WM_COMMAND,ID_OK,0);
									}	
									else
									{
										vk_return =TRUE;
									}
								}
								break;
								////
						default:
								return DefWindowProc(hwnd,msg,wParam,lParam);
								
						
					}
					
				}
				break;
				/////
				
		case	WM_NOTIFY:
				{
					U16	code,id;
					int i;
					
					code	=HIWORD(wParam);
					id		=LOWORD(wParam);
					
					if(code != BN_CLICKED)
					{
						break;
					}
					switch(id)
					{
						case	ID_CANCEL:
						
								Result =ID_CANCEL;
								SendMessage(hwnd,WM_CLOSE,0,0);
								break;
								////
								
						case	ID_CLR:
						
								memset(sel_text,0,sizeof(sel_text));
							
								Result =ID_CLR;
								SendMessage(hwnd,WM_CLOSE,0,0);		//关闭窗口
								break;
								////

						case	ID_OK:
																																
								i=SendMessage(GetDlgItem(hwnd,ID_LIST),LB_GETCURSEL,0,0);
								SendMessage(GetDlgItem(hwnd,ID_LIST),LB_GETTEXT,i,(LPARAM)sel_text);
													
								Result =ID_OK;
								SendMessage(hwnd,WM_CLOSE,0,0);		//关闭窗口
								break;
								////						
					}				
					
				}
				break;
				////////
		
		default:
		return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
		
	return	WM_NULL;
}

/*============================================================================*/
/*============================================================================*/

int	SetAutoRun(const char *auto_run_file)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;
	////

	//如果按下了VK_RETURN键,则需要弹起后才有效,以防止误操作.
	if(GetKeyState(VK_RETURN))
	{
		vk_return=FALSE;
	}
	else
	{
		vk_return=TRUE;
	}
	
//	PlaySound("B:test1.wav",PLAYSND_SINGLE);
//	PlaySound("B:test.wav",PLAYSND_LOOP|PLAYSND_ASYNC);

	Result=ID_CANCEL;
	
	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
		
	hwnd	=CreateWindowEx(WS_EX_NODRAG,&wcex,L"Set AutoRun",WS_OVERLAPPEDWINDOW,
								0,0,GUI_XSIZE,GUI_YSIZE,
								NULL,0,hInst,NULL);
								
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
		
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	switch(Result)
	{
	
		case	ID_CLR:
				{
					X_FILE *fp;
					char *buf;
					
					buf =vmalloc(256);
					if(buf!=NULL)
					{					
						fp =x_fopen(auto_run_file,"wb+");
						if(fp!=NULL)
						{
							memset(buf,0,256);
						
							x_fwrite(buf,1,256,fp);
							x_fclose(fp);
														
						}
						vfree(buf);
					}
				
				}
				break;
				////
				
		case	ID_OK:
				{
					X_FILE *fp;
					char *buf;
					
					buf =vmalloc(256);
					if(buf!=NULL)
					{					
						fp =x_fopen(auto_run_file,"wb+");
						if(fp!=NULL)
						{
							memset(buf,0,256);
							x_wcstombs_cp936(buf,sel_text,256);
							x_fwrite(buf,1,256,fp);
							x_fclose(fp);
														
						}
						vfree(buf);
					}
				}	
				break;
				////															

	}
	
	return	TRUE;	
}

int	ExecuAutoRun(const char *auto_run_file)
{
	X_FILE *fp;
	char buf[256];
		
	fp =x_fopen(auto_run_file,"rb");
		
	if(fp!=NULL)
	{
		x_fread(buf,1,256,fp);
			
		SYS_execu(buf,NULL,NULL);
			
		x_fclose(fp);
		return	TRUE;
	
	}
		
	return FALSE;
		
}

/*============================================================================*/

/*=========================================================================================*/

