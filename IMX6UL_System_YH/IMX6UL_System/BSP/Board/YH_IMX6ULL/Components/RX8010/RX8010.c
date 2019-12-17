

#include "BSP.h"
#include "RX8010.h"

/*=================================================================================*/
// RX8010 Max IIC_SCL =400KHz;

#define	IIC_SPEED	IIC_SPEED_400KHZ
static const struct i2c_ops *pIIC= NULL;


#define	RX8010_WT_ADDR	0x64
#define	RX8010_RD_ADDR	0x65


//// regs
#define	REG_SEC		0x10 //BCD:00-59
#define	REG_MIN		0x11 //BCD:00-59
#define	REG_HOUR	0x12 //BCD:00-23
#define	REG_WEEK	0x13 //BCD:1-7
#define	REG_DATE	0x14 //BCD:00-31
#define	REG_MONTH	0x15 //BCD:01-12
#define	REG_YEAR	0x16 //BCD:00-99
//#define	REG_OUT		0x07

//#define	REG_CON     0x0E
//#define	REG_SR		0x0F


/*=================================================================================*/

int RX8010_IsOK=FALSE;
static BOOL init=FALSE;
/*=================================================================================*/
/*=================================================================================*/

static void write_reg(u8 reg,u8 val)
{
	if(pIIC->Open(IIC_SPEED))
	{
		pIIC->Start();

		if(!pIIC->WriteByte(RX8010_WT_ADDR,IIC_ACK))
		{
			RX8010_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(val,IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}

static u8	read_reg(u8 reg)
{
	u8 val=0;

	if(pIIC->Open(IIC_SPEED))
	{
		pIIC->Start();

		if(!pIIC->WriteByte(RX8010_WT_ADDR,IIC_ACK))
		{
			RX8010_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return 0;
		}
		RX8010_IsOK=TRUE;
		pIIC->WriteByte(reg,IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(RX8010_RD_ADDR,IIC_ACK);

		val=pIIC->ReadByte(IIC_NoACK);

		pIIC->Stop();
		pIIC->Close();


	}

	return val;
}

static u8	read_regs(u8 reg,u8 *buf,u8 n)
{
	u8 i=0;

	if(pIIC->Open(IIC_SPEED))
	{
		pIIC->Start();

		if(!pIIC->WriteByte(RX8010_WT_ADDR,IIC_ACK))
		{
			RX8010_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return 0;
		}
		RX8010_IsOK=TRUE;
		pIIC->WriteByte(reg,IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(RX8010_RD_ADDR,IIC_ACK);

		for(i=0;i<n;i++)
		{
			if(i<(n-1))
			{
				buf[i] =pIIC->ReadByte(IIC_ACK);
			}
			else
			{
				buf[i] =pIIC->ReadByte(IIC_NoACK);
			}
		}

		pIIC->Stop();
		pIIC->Close();


	}

	return i;
}

/*=================================================================================*/

static u8 BCDtoDEC(u8 bcd)
{
	return (bcd>>4)*10+(bcd&0x0F);
}

static u8 DECtoBCD(u8 dec)
{
	u8 val;

	val  =(dec/10)<<4;
	val += dec%10;
	return val;
}

/*=================================================================================*/

BOOL RX8010_GetDate(u8 *year,u8 *mon,u8 *day)
{
	volatile u8 buf[4];

	if(init==FALSE)
	{
		return FALSE;
	}

	buf[0]=read_reg(REG_DATE);
	buf[1]=read_reg(REG_MONTH);
	buf[2]=read_reg(REG_YEAR);

	*year =BCDtoDEC(buf[2]);
	*mon  =BCDtoDEC(buf[1]);
	*day  =BCDtoDEC(buf[0]);
	return TRUE;
}

////////

BOOL RX8010_GetTime(u8 *hour,u8 *min,u8 *sec)
{
	volatile u8 buf[4];

	if(init==FALSE)
	{
		return FALSE;
	}

	buf[0]=read_reg(REG_SEC);
	buf[1]=read_reg(REG_MIN);
	buf[2]=read_reg(REG_HOUR);

	*hour =BCDtoDEC(buf[2]);
	*min  =BCDtoDEC(buf[1]);
	*sec  =BCDtoDEC(buf[0]);

	return TRUE;

}

/*=================================================================================*/

BOOL RX8010_SetDate(u8 year,u8 mon,u8 day)
{
	volatile u8 buf[8];
	int i;

	if(init==FALSE)
	{
		return FALSE;
	}

	write_reg(REG_DATE,DECtoBCD(day));
	write_reg(REG_MONTH,DECtoBCD(mon));
	write_reg(REG_YEAR,DECtoBCD(year));

#if 0
	for(i=0;i<8;i++)
	{     //BCD处理
	     tmp=time_buf1[i]/10;
	     time_buf[i]=time_buf1[i]%10;
	     time_buf[i]=time_buf[i]+tmp*16;
	}
	//        write_reg(0x00,0x00);                            //关闭写保护
	//        write_reg(0x01,0x00);                                //暂停
	write_reg(0x10,time_buf[7]);                //秒
	write_reg(0x11,time_buf[6]);                //分
	write_reg(0x12,time_buf[5]);                //时
	write_reg(0x13,time_buf[4]);                //周
	write_reg(0x14,time_buf[3]);                //日
	write_reg(0x15,time_buf[2]);                //月
	write_reg(0x16,time_buf[1]);                //年
	//        write_reg(0x00,0x00);                        //打开写保护

	//write_reg(REG_DATE,DECtoBCD(day));
	//write_reg(REG_MONTH,DECtoBCD(mon));
	//write_reg(REG_YEAR,DECtoBCD(year));
#endif
	return TRUE;
}

////////

BOOL RX8010_SetTime(u8 hour,u8 min,u8 sec)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	write_reg(REG_SEC,DECtoBCD(sec));
	write_reg(REG_MIN,DECtoBCD(min));
	write_reg(REG_HOUR,DECtoBCD(hour));

	return TRUE;
}

/*=================================================================================*/
void	RX8010_Test(void);

void	RX8010_Init(const struct i2c_ops *iic)
{
	int i;
	u8 y,d,h,m,s;

	pIIC =iic;
	init =TRUE;

	RX8010_GetTime(&h,&m,&s);
	if(RX8010_IsOK)
	{
		DebugOutput("RX8010_Init OK!\r\n");
	}
	else
	{
		DebugOutput("RX8010_Init Failed!\r\n");
	}
//	RX8010_Test();
}

/*=================================================================================*/
void	RX8010_Test(void)
{
	u8 y,d,h,m,s;
	int i;

	DebugOutput("RX8010_Test.\r\n");


//	RX8010_SetDate(17,1,1);
//	RX8010_SetTime(0,0,0);

	i=100;
	while(i-- > 0)
	{
		DebugOutput("RX8010(%d,%d)\r\n",init,RX8010_IsOK);
		RX8010_GetDate(&y,&m,&d);
		DebugOutput("RX8010 Date:%02d-%02d-%02d\r\n",y,m,d);
		RX8010_GetTime(&h,&m,&s);
		DebugOutput("RX8010 Time:%d:%02d:%02d\r\n",h,m,s);

		SYS_msleep(500);
	}
}

/*=================================================================================*/

