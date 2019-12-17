#ifndef __AP3216C_H__
#define __AP3216C_H__

#include "BSP.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
#define AP3216C_ADDR    0X3C	//AP3216C器件IIC地址(左移了一位)


BOOL AP3216C_Init(const struct i2c_ops *iic);
void AP3216C_ReadData(u16 *ir,u16 *ps,u16 *als);

#endif
