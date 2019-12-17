
#include	"GUI_Drv.h"
#include    "BSP.h"

#ifdef GUI_KEYBOARD_EN

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

static	GPIO_REG *gpio =GPIO_REG_VA_BASE;

/*============================================================================*/


#define	KS0_L()	//gpio->GPFDAT&=~(1<<6)
#define	KS0_H()	//gpio->GPFDAT|=(1<<6)

#define	KS1_L()	//gpio->GPFDAT&=~(1<<7)
#define	KS1_H()	//gpio->GPFDAT|=(1<<7)

#define	KS2_L()	 
#define	KS2_H()	 

#define	KS3_L()	 
#define	KS3_H()	 


//#define	K0_DOWN()	((gpio->GPGDAT&(1<<4))==0)
#define	K1_DOWN()	((gpio->GPFDAT&(1<<0))==0)
#define	K2_DOWN()	((gpio->GPFDAT&(1<<1))==0)
#define	K3_DOWN()	((gpio->GPFDAT&(1<<2))==0)
#define	K4_DOWN()	((gpio->GPFDAT&(1<<3))==0)
#define	K5_DOWN()	((gpio->GPFDAT&(1<<4))==0)

/*============================================================================*/
/*============================================================================*/


static	U8	key_state[256];

/*============================================================================*/

static	void	delay_us(int time)
{
	int	 i;
	
	while(time--)
	{
		for(i=500;i>0;i--);
	}
}
/*============================================================================*/


/*============================================================================*/

static	void	key_down_handler(int vkey)
{
//??	BacklightTimerReset();

	KeyboardInput(vkey,KEY_STATE_DOWN);
	//DebugPrintf("key_down -> %02XH\r\n",vkey);

}

static	void	key_up_handler(int vkey)
{

//	if(key_state[vkey]==KEY_STATE_DOWN)
	{
//		key_state[vkey]	=KEY_STATE_UP;
	
		KeyboardInput(vkey,KEY_STATE_UP);
		//DebugPrintf("key_up -> %02XH\r\n",vkey);
	}

}
/*============================================================================*/


static	int		_IsKeyDown(void)
{
  
	if(K1_DOWN())	return	1;
	if(K2_DOWN())	return	1;
	if(K3_DOWN())	return	1;
	if(K4_DOWN())	return	1;
	if(K5_DOWN())	return	1;
	
  
	return 0;
}

/*============================================================================*/

static	void	_KeyboardScan(void)
{


	
	if(K1_DOWN())
	{
		key_down_handler(VK_RETURN);
	}
	else
	{
		key_up_handler(VK_RETURN);
	}
	
	
	if(K2_DOWN())
	{
		key_down_handler(VK_RIGHT);
	}
	else	
	{
		key_up_handler(VK_RIGHT);
	}
	
	if(K3_DOWN())
	{
		key_down_handler(VK_LEFT);
	}
	else
	{
		key_up_handler(VK_LEFT);
	}
	
	if(K4_DOWN())
	{
		key_down_handler(VK_DOWN);
	}
	else
	{
		key_up_handler(VK_DOWN);
	}
	
	if(K5_DOWN())
	{
		key_down_handler(VK_UP);
	}
	else
	{
		key_up_handler(VK_UP);
	}
	

}

/*============================================================================*/
/*============================================================================*/
static	int	key_time=0;

void	gdrvKeyBoardHandler(void)
{

#if 1
	
	if(_IsKeyDown())
	{
		if(key_time<0x00FFFFFF)
		{
			key_time++;
			
			if(key_time==1)
			{
				_KeyboardScan();
			}
			
			if(key_time>25)
			{
				_KeyboardScan();
			}
		}
	}
	else
	{
		if(key_time>1)
		{
			_KeyboardScan();
			key_time=0;
		}
	}
#endif

}

/*============================================================================*/


BOOL	gdrvKeyboardInit(void)
{
	gpio =GPIO_REG_VA_BASE;
	
/* For Helper2416

	KIN1	->GPF0
	KIN2	->GPF1
	KIN3	->GPF2
	KIN4	->GPF3
	KIN5	->GPF4
	
*/
#if 1
		
	GPIO_SetMode(GPF,0,GPIO_INPUT|GPIO_PULL_UP);
	GPIO_SetMode(GPF,1,GPIO_INPUT|GPIO_PULL_UP);
	GPIO_SetMode(GPF,2,GPIO_INPUT|GPIO_PULL_UP);
	GPIO_SetMode(GPF,3,GPIO_INPUT|GPIO_PULL_UP);
	GPIO_SetMode(GPF,4,GPIO_INPUT|GPIO_PULL_UP);
	
	DebugPrintf("gdrvKeyboardInit.\r\n");
#endif

	return	TRUE;

}

/*============================================================================*/
/*============================================================================*/
#endif
