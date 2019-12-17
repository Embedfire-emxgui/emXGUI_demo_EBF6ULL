#include	"Include.h"

/*
 * GUI_DEMO.c
 *
 *  Created on: 2019年7月31日
 *      Author: liuwei
 */


static int prio=0;

static int _thread_1(void *p)
{
	volatile int t0,t1,time,i;
	volatile float f1,f2,f3;
	volatile float f4,f5,f6;

	SYS_thread_set_priority(NULL,prio++);

	f1 =4325.43f;
	f2 =124.f;

	while(1)
	{
		t0 =GUI_GetTickCount();
		f1=1.7f;
		f2=3.1435;
		f3=123.4;
		f4=1.2344;
		for(i=0;i<100;i++)
		{
			f5 =f2*f3/f4;
			SYS_msleep(10);
			f6 =f2+f3-f4;
			SYS_msleep(10);
			f2 += 0.2323f;
			f3 -= 1.232f;
			f4 += 1.1456f;
			f1 += f3;

		}
		f1 =f1+f2+f3+f4+f5+f6;
		t1 =GUI_GetTickCount();
		time = t1-t0;

		if(f1!=6175.7124f)
		{
			SYS_msleep(20);
		}
		SYS_msleep(10);

	}
}

/*=========================================================================================*/

static volatile u32 stk_total,stk_max_used,stk_rem_bytes;
void	HAL_StackInfo(u32 *pTot,u32 *pMaxUsed,u32 *pRem);

void GUI_Demo(void)
{
#if 0
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
	SYS_thread_create(_thread_1,NULL,4096,NULL,0);
#endif

	GUI_DEMO_Listbox_Loop();
	GUI_DEMO_Button_Transparent();
	GUI_DEMO_Listbox_Add_Del();
	GUI_DEMO_MultiWindow2();
	GUI_DEMO_Rotate();
	GUI_DEMO_MultiWindow();
	//GUI_DEMO_TTF();
	GUI_DEMO_DrawText();
	GUI_DEMO_ScrollbarOwnerDraw();
	GUI_DEMO_DrawBase();
	GUI_DEMO_MEMDC_Blt();
	GUI_DEMO_Rotate();
	GUI_DEMO_DrawAA();
//	GUI_DEMO_PNG();
//	GUI_DEMO_Jpeg();
//	GUI_DEMO_BMP();

	//DrawBitmap_Test_Fish();
//	GUI_DEMO_DrawMeter();
	GUI_DEMO_MEMDC_Blt();
	GUI_DEMO_ShowWave2();
	GUI_DEMO_ShowWave();
	GUI_DEMO_MultiWindow();
	GUI_DEMO_MoveWindow();
	GUI_DEMO_Listbox_OwnerDraw();
	GUI_DEMO_OwnerDC();
//	GUI_DEMO_CustomWidget();
	//ChessMain(NULL,NULL);
	//GamepaperMain(NULL,NULL);
	//GUI_DEMO_TTF();
	//GUI_DEMO_RectSpeed();
//	GUI_DEMO_BMP_Speed();
	GUI_DEMO_BitBlt_Speed();
	GUI_DEMO_TextOut();
//	SensorTest_WinMain(NULL);
//	GUI_DEMO_Drawbitmap();
//	GUI_DEMO_Rotatebitmap();
	GUI_DEMO_ScaleBitmap();
//	DrawBitmap_Test_Fish();

	GUI_DEMO_Textbox();
	GUI_DEMO_Button();
	GUI_DEMO_Checkbox();
	GUI_DEMO_Radiobox();
	GUI_DEMO_Progressbar();
	GUI_DEMO_Scrollbar();
	GUI_DEMO_Listbox();
	GUI_DEMO_Messagebox();


	//HAL_StackInfo(&stk_total,&stk_max_used,&stk_rem_bytes);

}
/*=========================================================================================*/

#if 1
static int app_run=FALSE;
static int app_thread(void *argv)
{
	GUI_Demo();
	app_run=FALSE;
	GUI_Thread_Delete(GUI_GetCurThreadHandle());
}

void	app_GUI_DEMO(HWND hwndPrev)
{
	if(app_run==FALSE)
	{
		app_run =TRUE;
		GUI_Thread_Create(app_thread,hwndPrev,8192,NULL,0);
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

/*=========================================================================================*/
