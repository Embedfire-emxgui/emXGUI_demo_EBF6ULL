#include "BSP.h"
#include "NAU8822.h"

//#define	MASTER_MODE	1

static const struct i2c_ops *pIIC=NULL;

BOOL	NAU8822_IsOK=FALSE;

//WM8978默认参数
_wm8978_obj wm8978set=
{
	50,			//音量:50
	0,0,0,0,0,	//频率标志设为0
	0,0,0,0,0,	//中心频率增益表全0
	0,			//3D效果为0
	1,			//开启板载喇叭		  	 
	0,			 
};

//WM8978数据保存在:SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)之后
//读取WM8978数据
//wm8978dev:wm8978数据
void wm8978_read_para(_wm8978_obj * wm8978dev)
{
	//AT24CXX_Read(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings),(u8*)wm8978dev,sizeof(_wm8978_obj));
}
//写入WM8978数据
//wm8978dev:wm8978数据
void wm8978_save_para(_wm8978_obj * wm8978dev)
{										   
//    OS_CPU_SR cpu_sr=0;
//	OS_ENTER_CRITICAL();	//进入临界区(无法被中断打断)
//	AT24CXX_Write(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings),(u8*)wm8978dev,sizeof(_wm8978_obj));
//	OS_EXIT_CRITICAL();		//退出临界区(可以被中断打断)
} 

//WM8978寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为WM8978的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写WM8978寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:WM8978的寄存器值是9位的,所以要用u16来存储. 
static u16 NAU8822_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 

//WM8978写寄存器
//reg:寄存器地址
//val:要写入寄存器的值
//返回值:0,成功;
//    其他,错误代码
static BOOL NAU8822_Write_Reg(u8 reg,u16 val)
{
	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(NAU8822_ADDR,IIC_ACK))//发送器件地址+写命令
	{
		NAU8822_IsOK=FALSE;
		pIIC->Stop();

		pIIC->Close();
		return FALSE;
	}
	NAU8822_IsOK=TRUE;
	pIIC->WriteByte((reg<<1)|((val>>8)&0x01),IIC_ACK);//写寄存器地址+数据的最高位
	pIIC->WriteByte(val&0xFF,IIC_ACK);	//发送数据
	pIIC->Stop();

	pIIC->Close();
	NAU8822_REGVAL_TBL[reg]=val;	//保存寄存器值到本地
	return TRUE;	
}

//WM8978读寄存器
//就是读取本地寄存器值缓冲区内的对应值
//reg:寄存器地址
//返回值:寄存器值
static u16 NAU8822_Read_Reg(u8 reg)
{
	return NAU8822_REGVAL_TBL[reg];
}


//WM8978初始化
//返回值:0,初始化正常
//    其他,错误代码
BOOL NAU8822_Init(const struct i2c_ops *iic)
{
	u8 res; 

	pIIC =iic;
	res=NAU8822_Write_Reg(0,0x000);	/* Reset all registers */
	if(!res)
	{
		ErrorOutput("NAU8822_Failed!\r\n");
		return FALSE;			//发送指令失败,WM8978异常
	}
	SYS_msleep(100);
	
	//input source is MIC
	NAU8822_Write_Reg(1,  0x03F);
	NAU8822_Write_Reg(2,  0x1BF);   /* Enable L/R Headphone, ADC Mix/Boost, ADC */
	NAU8822_Write_Reg(3,  0x07F);   /* Enable L/R main mixer, DAC */
	NAU8822_Write_Reg(4,  0x010);   /* 16-bit word length, I2S format, Stereo */
	NAU8822_Write_Reg(5,  0x000);   /* Companding control and loop back mode (all disable) */
#ifndef MASTER_MODE
	NAU8822_Write_Reg(6,  0x1AD);   /* Divide by 6, 16K */
	NAU8822_Write_Reg(7,  0x006);   /* 16K for internal filter coefficients */
#endif
	NAU8822_Write_Reg(10, 0x008);   /* DAC soft mute is disabled, DAC oversampling rate is 128x */
	NAU8822_Write_Reg(14, 0x108);   /* ADC HP filter is disabled, ADC oversampling rate is 128x */
	NAU8822_Write_Reg(15, 0x1EF);   /* ADC left digital volume control */
	NAU8822_Write_Reg(16, 0x1EF);   /* ADC right digital volume control */
	NAU8822_Write_Reg(44, 0x033);   /* LMICN/LMICP is connected to PGA */
	NAU8822_Write_Reg(50, 0x001);   /* Left DAC connected to LMIX */
	NAU8822_Write_Reg(51, 0x001);   /* Right DAC connected to RMIX */
	DebugOutput("NAU8822_OK!\r\n");

	DebugOutput("NAU8822: %d\r\n",NAU8822_IsOK);

	return TRUE;
} 









