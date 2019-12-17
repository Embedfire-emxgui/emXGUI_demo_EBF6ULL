/*
 * PCA9555.c
 *
 *  Created on: 2017Äê6ÔÂ3ÈÕ
 *      Author: liuwei
 */

#include "BSP.h"
#include "PCA9555.h"

/*=================================================================================*/

int PCA9555_IsOK=FALSE;

/*=================================================================================*/

static const struct i2c_ops *pIIC= &I2C_GPH9_GPH8;

#define	PCA9555_ADDR	0x40


static	volatile u8 reg_buf[8];

/*=================================================================================*/
/*=================================================================================*/

void PCA9555_Init(const struct i2c_ops *iic)
{
	pIIC =iic;
}

/*=================================================================================*/

void PCA9555_SetRegValue(u8 reg,u8 val)
{
	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		PCA9555_IsOK=TRUE;
		reg_buf[reg] = val;

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(reg_buf[reg],IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}

void PCA9555_SetRegBits(u8 reg,u8 bits)
{
	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		PCA9555_IsOK=TRUE;
		reg_buf[reg] |= bits;

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(reg_buf[reg],IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}

void PCA9555_ClrRegBits(u8 reg,u8 bits)
{
	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		PCA9555_IsOK=TRUE;
		reg_buf[reg] &= ~bits;

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(reg_buf[reg],IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}


/*=================================================================================*/

static const u8 out_reg[2]={PCA9555_OUT0,PCA9555_OUT1};

void	PCA9555_SetMode(u8 port,u8 pin,u8 dir)
{

}

/*=================================================================================*/

void	PCA9555_SetPin(PCA9555_PORT port,u8 pin)
{
	u8 reg,bits;

	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		PCA9555_IsOK=TRUE;
		reg =out_reg[port];
		bits =(1<<pin);

		reg_buf[reg] |= bits;

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(reg_buf[reg],IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}

void	PCA9555_ClrPin(PCA9555_PORT port,u8 pin)
{
	u8 reg,bits;

	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return;
		}

		PCA9555_IsOK=TRUE;
		reg =out_reg[port];
		bits =(1<<pin);

		reg_buf[reg] &= ~bits;

		pIIC->WriteByte(reg,IIC_ACK);
		pIIC->WriteByte(reg_buf[reg],IIC_ACK);

		pIIC->Stop();
		pIIC->Close();
	}

}

u8 PCA9555_GetPin(PCA9555_PORT port,u8 pin)
{
	u8 reg,bits;

	if(pIIC->Open())
	{
		pIIC->Start();

		if(!pIIC->WriteByte(PCA9555_ADDR,IIC_ACK))
		{
			PCA9555_IsOK=FALSE;
			pIIC->Stop();
			pIIC->Close();
			return 0;
		}

		PCA9555_IsOK=TRUE;
		pIIC->WriteByte(reg,IIC_ACK);

		pIIC->Start();
		pIIC->WriteByte(PCA9555_ADDR+1,IIC_ACK);

		bits=pIIC->ReadByte(IIC_NoACK);

		pIIC->Stop();
		pIIC->Close();

		if(bits&(1<<pin))
		{
			return 1;
		}

	}
	return 0;

}

/*=================================================================================*/

