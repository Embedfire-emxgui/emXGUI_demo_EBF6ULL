#ifndef __PCA9555_H__
#define __PCA9555_H__

#ifdef __CPLUSPLUS
extern "C" {
#endif

/*=================================================================================*/
////regs
#define	PCA9555_IN0		0
#define	PCA9555_IN1		1
#define	PCA9555_OUT0	2
#define	PCA9555_OUT1	3
#define	PCA9555_POL0	4
#define	PCA9555_POL1	5
#define	PCA9555_CON0	6
#define	PCA9555_CON1	7


/*=================================================================================*/

extern int PCA9555_IsOK;

/*=================================================================================*/

void PCA9555_Init(const struct i2c_ops *iic);
void PCA9555_SetRegValue(u8 reg,u8 val);
void PCA9555_SetRegBits(u8 reg,u8 bits);
void PCA9555_ClrRegBits(u8 reg,u8 bits);

/*=================================================================================*/
typedef	enum
{
	PCA9555_P0 =0,
	PCA9555_P1 =1
}PCA9555_PORT;

void	PCA9555_SetPin(PCA9555_PORT port,u8 pin);
void	PCA9555_ClrPin(PCA9555_PORT port,u8 pin);
u8 		PCA9555_GetPin(PCA9555_PORT port,u8 pin);

/*=================================================================================*/

#ifdef __CPLUSPLUS
}
#endif

#endif /* __PCA9555_H__ */
