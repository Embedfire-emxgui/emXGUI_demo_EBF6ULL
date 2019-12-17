

#include "BSP.h"

/*=================================================================================*/

#define	IIC_SPEED	IIC_SPEED_400KHZ

static const struct i2c_ops *pIIC= NULL;

#define	ISL1208_ADDR	0xDE

//// regs
#define	REG_SEC		0x00 //BCD:0-59
#define	REG_MIN		0x01 //BCD:0-59
#define	REG_HOUR	0x02 //BCD:0-23
#define	REG_DAY		0x03 //BCD:1-31
#define	REG_MON		0x04 //BCD:1-12
#define	REG_YEAR	0x05 //BCD:0-99
#define	REG_WEEK	0x06 //BCD:0-6

#define	REG_SR		0x07
#define	REG_INT		0x08
#define	REG_ATR		0x0A
#define	REG_DTR		0x0B

#define	REG_AR_SEC	0x0C //BCD:0-59
#define	REG_AR_MIN	0x0D //BCD:0-59
#define	REG_AR_HOUR	0x0E //BCD:0-23
#define	REG_AR_DAY	0x0F //BCD:1-31
#define	REG_AR_MON	0x10 //BCD:1-12
#define	REG_AR_WEEK	0x11 //BCD:0-6

/*=================================================================================*/

int ISL1208_IsOK=FALSE;
static BOOL init=FALSE;
/*=================================================================================*/
/*=================================================================================*/

static void write_reg(u8 reg,u8 val)
{
	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(ISL1208_ADDR,IIC_ACK))
		{
			ISL1208_IsOK=FALSE;
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

	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(ISL1208_ADDR,IIC_ACK))
		{
			ISL1208_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return 0;
		}
		ISL1208_IsOK=TRUE;
		pIIC->WriteByte(reg,IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(ISL1208_ADDR+1,IIC_ACK);

		val=pIIC->ReadByte(IIC_NoACK);

		pIIC->Stop();
		pIIC->Close();


	}

	return val;
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

static void RTC_Write_EN(BOOL bEN)
{
	u8 sr;

	sr=read_reg(REG_SR);
	if(bEN)
	{
		sr |=   (1<<4);
	}
	else
	{
		sr &= ~(1<<4);
	}
	write_reg(REG_SR,sr);
}

/*=================================================================================*/

BOOL ISL1208_GetDate(u8 *year,u8 *mon,u8 *day)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	if(year!=NULL)
	{
		*year =BCDtoDEC(read_reg(REG_YEAR)&0xFF);
	}
	if(mon!=NULL)
	{
		*mon =BCDtoDEC(read_reg(REG_MON)&0x1F);
	}
	if(day!=NULL)
	{
		*day =BCDtoDEC(read_reg(REG_DAY)&0x3F);
	}

	return TRUE;
}

////////

BOOL ISL1208_GetTime(u8 *hour,u8 *min,u8 *sec)
{
	volatile u8 r,sr;

	if(init==FALSE)
	{
		return FALSE;
	}

	sr =read_reg(REG_SR);

	if(hour!=NULL)
	{
		r=read_reg(REG_HOUR);
		if(r&(1<<7))
		{// 24Hour Format
			r&=0x3F;
		}
		else
		{// 12Hour Format
			r&=0x1F;
		}

		*hour =BCDtoDEC(r);
	}
	if(min!=NULL)
	{
		r=read_reg(REG_MIN);
		r&=0x7F;
		*min =BCDtoDEC(r);
	}
	if(sec!=NULL)
	{
		r=read_reg(REG_SEC);
		r&=0x7F;
		*sec =BCDtoDEC(r);
	}

	return TRUE;
}

/*=================================================================================*/

BOOL ISL1208_SetDate(u8 year,u8 mon,u8 day)
{

	if(init==FALSE)
	{
		return FALSE;
	}

	RTC_Write_EN(TRUE);
	write_reg(REG_YEAR,DECtoBCD(year));
	write_reg(REG_MON,DECtoBCD(mon));
	write_reg(REG_DAY,DECtoBCD(day));
	RTC_Write_EN(FALSE);

	return TRUE;
}

////////

BOOL ISL1208_SetTime(u8 hour,u8 min,u8 sec)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	RTC_Write_EN(TRUE);
	write_reg(REG_HOUR,DECtoBCD(hour)|(1<<7)); //24-hour format
	write_reg(REG_MIN,DECtoBCD(min));
	write_reg(REG_SEC,DECtoBCD(sec));
	RTC_Write_EN(FALSE);

	return TRUE;
}

/*=================================================================================*/

void	ISL1208_Init(const struct i2c_ops *iic)
{
	int i;
	u8 sr,y,d,h,m,s;

	pIIC =iic;
	init =TRUE;

	RTC_Write_EN(TRUE);
	sr =read_reg(REG_HOUR);
	sr |=(1<<7);
	write_reg(REG_HOUR,sr);
	RTC_Write_EN(FALSE);

	write_reg(REG_SR,(1<<7)|(0<<6)|(1<<4)|(1<<1));

	sr =read_reg(REG_SR);
	if(sr&(1<<0))
	{//时间数据无效，必须重新设置为有效的时间.
		DebugOutput("ISL1208_SR: %02X!\r\n",sr);

		//write_reg(REG_SR,(1<<7)|(0<<6)|(1<<4)|(1<<1));

		y=17;
		m=1;
		d=1;
		ISL1208_SetDate(y,m,d);

		h=0;
		m=0;
		s=0;
		ISL1208_SetTime(h,m,s);

		write_reg(REG_WEEK,0);
	}

	ISL1208_GetTime(&h,&m,&s);

	if(ISL1208_IsOK)
	{
		DebugOutput("ISL1208_Init OK!\r\n");
	}
	else
	{
		DebugOutput("ISL1208_Init Failed!\r\n");
	}

}

/*=================================================================================*/
void	ISL1208_Test(void)
{
	u8 y,d,h,m,s;
	int i;

	DebugOutput("ISL1208_Test.\r\n");

	i=10;
	while(i-->0)
	{
		DebugOutput("ISL1208(%d,%d) SR:%2XH\r\n",init,ISL1208_IsOK,read_reg(REG_SR));
		ISL1208_GetDate(&y,&m,&d);
		DebugOutput("ISL1208 Date:%02d-%02d-%02d\r\n",y,m,d);
		ISL1208_GetTime(&h,&m,&s);
		DebugOutput("ISL1208 Time:%d:%02d:%02d\r\n",h,m,s);

		SYS_msleep(500);
	}
}

/*=================================================================================*/

