/*
 * PCA9555.c
 *
 *  Created on: 2017Äê6ÔÂ3ÈÕ
 *      Author: liuwei
 */

#include "BSP.h"

/*=================================================================================*/

static const struct i2c_ops *pIIC= NULL;

#define	PCF8563_ADDR	0xA2

//// regs(BCD format)
#define	REG_CFG		0x00
#define	REG_SEC		0x02 //BCD:00-59
#define	REG_MIN		0x03 //BCD:00-59
#define	REG_HOUR	0x04 //BCD:00-23
#define	REG_DAY		0x05 //BCD:01-31
#define	REG_WEEK	0x06 //BCD:0-6
#define	REG_MON		0x07 //BCD:01-12
#define	REG_YEAR	0x08 //BCD:00-99

#define	REG_AR_MIN	0x09 //BCD:00-59
#define	REG_AR_HOUR	0x0A //BCD:00-23
#define	REG_AR_DAY	0x0B //BCD:01-31
#define	REG_AR_WEEK	0x0C //BCD:0-6

/*=================================================================================*/

int PCF8563_IsOK=FALSE;
static BOOL init=FALSE;
/*=================================================================================*/
/*=================================================================================*/

static void write_reg(u8 reg,u8 val)
{
	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCF8563_ADDR,IIC_ACK))
		{
			PCF8563_IsOK=FALSE;
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

		if(!pIIC->WriteByte(PCF8563_ADDR,IIC_ACK))
		{
			PCF8563_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return 0;
		}
		PCF8563_IsOK=TRUE;
		pIIC->WriteByte(reg,IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(PCF8563_ADDR+1,IIC_ACK);

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

BOOL PCF8563_GetDate(u8 *year,u8 *mon,u8 *day)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	if(year!=NULL)
	{
		*year =BCDtoDEC(read_reg(REG_YEAR)&0x7F);
	}
	if(mon!=NULL)
	{
		*mon =BCDtoDEC(read_reg(REG_MON)&0x7F);
	}
	if(day!=NULL)
	{
		*day =BCDtoDEC(read_reg(REG_DAY)&0x7F);
	}

	return TRUE;
}

////////

BOOL PCF8563_GetTime(u8 *hour,u8 *min,u8 *sec)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	if(hour!=NULL)
	{
		*hour =BCDtoDEC(read_reg(REG_HOUR)&0x7F);
	}
	if(min!=NULL)
	{
		*min =BCDtoDEC(read_reg(REG_MIN)&0x7F);
	}
	if(sec!=NULL)
	{
		*sec =BCDtoDEC(read_reg(REG_SEC)&0x7F);
	}

	return TRUE;
}

/*=================================================================================*/

BOOL PCF8563_SetDate(u8 year,u8 mon,u8 day)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	write_reg(REG_YEAR,DECtoBCD(year));
	write_reg(REG_MON,DECtoBCD(mon));
	write_reg(REG_DAY,DECtoBCD(day));

	return TRUE;
}

////////

BOOL PCF8563_SetTime(u8 hour,u8 min,u8 sec)
{
	if(init==FALSE)
	{
		return FALSE;
	}

	write_reg(REG_HOUR,DECtoBCD(hour));
	write_reg(REG_MIN,DECtoBCD(min));
	write_reg(REG_SEC,DECtoBCD(sec));

	return TRUE;
}

/*=================================================================================*/

void	PCF8563_Init(const struct i2c_ops *iic)
{
	pIIC =iic;
	write_reg(REG_CFG,0x00);
	init =TRUE;
	DebugOutput("PCF8563:%d\r\n",PCF8563_IsOK);
}

/*=================================================================================*/

