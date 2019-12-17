
#include	"BSP.h"
#include	"Kernel.h"
#include	"EMXGUI.h"

#include	"GUI_Drv.h"
#include	"GUI_Drv_Cfg.h"

#include	"x_libc.h"

/*===================================================================================*/
//For FT5406,FT6202
/*===================================================================================*/

#define		FT5406_IIC_SPEED	IIC_SPEED_100KHZ

static const struct i2c_ops *pIIC =&I2C_P1_30_P1_31;

/*===================================================================================*/


static BOOL hard_ok=TRUE;
static u16 ver=0;

/*===================================================================================*/

#define	MAX_TOUCH_NUM	5	//最大触摸点数量

/*===================================================================================*/
/*===================================================================================*/

//I2C读写命令
#define FT_CMD_WR 				0x70    	//写命令
#define FT_CMD_RD 				0x71		//读命令

//FT5336 部分寄存器定义
#define FT_DEVIDE_MODE 			0x00   		//FT5336模式控制寄存器
#define FT_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT_TP5_REG 				0X1B		//第五个触摸点数据地址


#define	FT_ID_G_LIB_VERSION		0xA1		//版本
#define FT_ID_G_MODE 			0xA4   		//FT5336中断模式控制寄存器
#define FT_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器

#define FTS_REG_ECC                                  0xCC
#define FTS_RST_CMD_REG2                             0xBC
#define FTS_READ_ID_REG                              0x90
#define FTS_ERASE_APP_REG                            0x61
#define FTS_ERASE_PARAMS_CMD                         0x63
#define FTS_FW_WRITE_CMD                             0xBF
#define FTS_REG_RESET_FW                             0x07
#define FTS_RST_CMD_REG1                             0xFC
#define LEN_FLASH_ECC_MAX                            0xFFFE

#define FTS_PACKET_LENGTH                            128
#define FTS_SETTING_BUF_LEN                          128

#define FTS_UPGRADE_LOOP                             30
#define AUTO_CLB_NEED                                1
#define AUTO_CLB_NONEED                              0
#define FTS_UPGRADE_AA                               0xAA
#define FTS_UPGRADE_55                               0x55
#define FTXXXX_INI_FILEPATH_CONFIG                   "/sdcard/"

/*===================================================================================*/

static void FT5406_Init_Port(void)
{
#if 1




#endif
}

static void delay_us(int n)
{
	volatile int i;
	while(n-- > 0)
	{
		for(i=0;i<2000;i++);
	}
}
/*===================================================================================*/

//向FT5336写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
static BOOL FT5406_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;

	pIIC->Open(FT5406_IIC_SPEED);

	pIIC->Start();
	if(!pIIC->WriteByte(FT_CMD_WR,IIC_ACK))	//发送写命令
	{
		pIIC->Stop();

		pIIC->Close();
		hard_ok =FALSE;
		return FALSE;
	}
	pIIC->WriteByte(reg&0XFF,IIC_ACK);   	//发送低8位地址
	for(i=0;i<len;i++)
	{
    	pIIC->WriteByte(buf[i],IIC_ACK);  	//发数据
	}
	pIIC->Stop();

    pIIC->Close();
	return TRUE;
}



//从FT5336读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度
static BOOL FT5406_RD_Reg(u8 reg,u8 *buf,u8 len)
{
	int i;


	pIIC->Open(FT5406_IIC_SPEED);

	pIIC->Start();
	if(!pIIC->WriteByte(FT_CMD_WR,IIC_ACK))	//发送写命令
	{
		hard_ok =FALSE;
		goto err;
	}

	if(!pIIC->WriteByte(reg&0XFF,IIC_ACK))   	//发送低8位地址
	{
		goto err;
	}

	pIIC->Start();
	if(!pIIC->WriteByte(FT_CMD_RD,IIC_ACK))   	//发送读命令
	{
		goto err;
	}

	for(i=0;i<len;i++)
	{
		if(i == (len-1))
		{
			buf[i] =pIIC->ReadByte(IIC_NoACK);
		}
		else
		{
			buf[i] =pIIC->ReadByte(IIC_ACK);
		}
	}
    pIIC->Stop();

    pIIC->Close();
    hard_ok =TRUE;
    return TRUE;

err:
	pIIC->Stop();
	pIIC->Close();
	return FALSE;

}


struct i2c_client{
	int addr;
};

static BOOL fts_i2c_write(u8 *buf,int len)
{
	int i;

	pIIC->Open(FT5406_IIC_SPEED);

	pIIC->Start();
    if(!pIIC->WriteByte(FT_CMD_WR,IIC_ACK))
    {
    	pIIC->Stop();

    	pIIC->Close();
    	return FALSE;
    }

	for (i = 0; i < len; i++)
	{
		pIIC->WriteByte(buf[i],IIC_ACK);
	}

	pIIC->Stop();

	pIIC->Close();
	return TRUE;
}

static BOOL fts_i2c_read( u8 *wr_buf,int wr_n, u8 *rd_buf, int rd_n)
{
	int i;


	pIIC->Open(FT5406_IIC_SPEED);

	pIIC->Start();
	if(!pIIC->WriteByte(FT_CMD_WR,IIC_ACK))	//发送写命令
	{
		hard_ok =FALSE;
		goto err;
	}

	for(i=0;i<wr_n;i++)
	{
		if(!pIIC->WriteByte(wr_buf[i],IIC_ACK))
		{
			goto err;
		}
	}

	pIIC->Start();
	if(!pIIC->WriteByte(FT_CMD_RD,IIC_ACK))   	//发送读命令
	{
		goto err;
	}

	for(i=0;i<rd_n;i++)
	{
		if(i == (rd_n-1))
		{
			rd_buf[i] =pIIC->ReadByte(IIC_NoACK);
		}
		else
		{
			rd_buf[i] =pIIC->ReadByte(IIC_ACK);
		}
	}
    pIIC->Stop();

    pIIC->Close();
    hard_ok =TRUE;
    return TRUE;

err:
	pIIC->Stop();
	pIIC->Close();
	return FALSE;

}

#if 1
/************************************************************************
* Name: fts_i2c_write_reg
* Brief: write register
* Input: i2c info, reg address, reg value
* Output: no
* Return: fail <0
***********************************************************************/
int fts_i2c_write_reg( u8 regaddr, u8 regvalue)
{
    u8 buf[2] = {0};

    buf[0] = regaddr;
    buf[1] = regvalue;
    return fts_i2c_write( buf, sizeof(buf));
}

/************************************************************************
* Name: fts_i2c_read_reg
* Brief: read register
* Input: i2c info, reg address, reg value
* Output: get reg value
* Return: fail <0
***********************************************************************/
int fts_i2c_read_reg( u8 regaddr, u8 *regvalue)
{
    return fts_i2c_read( &regaddr, 1, regvalue, 1);
}
#endif


#define	msleep SYS_msleep

/************************************************************************
* Name: fts_ft6x36gu_upgrade_use_buf
* Brief: fw upgrade
* Input: i2c info, file buf, file len
* Output: no
* Return: fail <0
***********************************************************************/
static int fts_ft6x36gu_upgrade_use_buf(u8 *pbt_buf, u32 dw_lenth)
{
    u8 reg_val[2] = {0};
    u32 i = 0;
    u32 packet_number;
    u32 j;
    u32 temp;
    u32 lenght;
    u32 fw_length;
    u8 packet_buf[FTS_PACKET_LENGTH + 6];
    u8 auc_i2c_write_buf[10];
    u8 upgrade_ecc;

    //FTS_FUNC_ENTER();

    /*if the first byte of app is not 0x02, the app is invaild, can not upgrade*/
    if (pbt_buf[0] != 0x02)
    {
        //FTS_ERROR("[UPGRADE]: app first byte is not 0x02. can not upgrade!!");
        return -1;
    }

    /*check app length*/
    if (dw_lenth > 0x11f)
    {
        fw_length = ((u32)pbt_buf[0x100]<<8) + pbt_buf[0x101];
        if (dw_lenth < fw_length)
        {
            //FTS_ERROR("[UPGRADE]: Fw length error!!");
            return -1;
        }
    }
    else
    {
        //FTS_ERROR("[UPGRADE]: Fw length error!!");
        return -1;
    }

    /*send upgrade commond*/
    for (i = 0; i < FTS_UPGRADE_LOOP; i++)
    {
        /*send 0xAA and 0x55 to fw(0xFC reg), and start upgrade*/
        fts_i2c_write_reg( FTS_RST_CMD_REG2, FTS_UPGRADE_AA);
        msleep(10);
        fts_i2c_write_reg( FTS_RST_CMD_REG2, FTS_UPGRADE_55);
        msleep(10);

        /*upgrade init in ROM*/
        auc_i2c_write_buf[0] = FTS_UPGRADE_55;
        fts_i2c_write( auc_i2c_write_buf, 1);
        auc_i2c_write_buf[0] = FTS_UPGRADE_AA;
        fts_i2c_write( auc_i2c_write_buf, 1);
        msleep(10);

        /*check run in ROM now*/
        auc_i2c_write_buf[0] = FTS_READ_ID_REG;
        auc_i2c_write_buf[1] = auc_i2c_write_buf[2] = auc_i2c_write_buf[3] = 0x00;
        reg_val[0] = 0x00;
        reg_val[1] = 0x00;
        fts_i2c_read( auc_i2c_write_buf, 4, reg_val, 2);
#if 0
        if (reg_val[0] == chip_types.bootloader_idh
            && reg_val[1] == chip_types.bootloader_idl)
        {
            //FTS_DEBUG("[UPGRADE]: get bootload id OK : ID1 = 0x%x,ID2 = 0x%x!!",reg_val[0], reg_val[1]);
            break;
        }
        else
        {
            //FTS_ERROR("[UPGRADE]: get bootload id error : ID1 = 0x%x,ID2 = 0x%x!!",reg_val[0], reg_val[1]);
            continue;
        }
#endif
    }

    if (i >= FTS_UPGRADE_LOOP)
    {
        //FTS_ERROR("[UPGRADE]: get bootload id error : ID1 = 0x%x,ID2 = 0x%x!!",reg_val[0], reg_val[1]);
        return -1;
    }

    /*erase app in flash*/
    //FTS_INFO("[UPGRADE]: erase app!!");
    auc_i2c_write_buf[0] = FTS_ERASE_APP_REG;
    fts_i2c_write( auc_i2c_write_buf, 1);
    msleep(2000);

    for (i = 0; i < 200; i++)
    {
        auc_i2c_write_buf[0] = 0x6a;
        auc_i2c_write_buf[1] = 0x00;
        auc_i2c_write_buf[2] = 0x00;
        auc_i2c_write_buf[3] = 0x00;
        reg_val[0] = 0x00;
        reg_val[1] = 0x00;
        fts_i2c_read( auc_i2c_write_buf, 4, reg_val, 2);
        if (0xb0 == reg_val[0] && 0x02 == reg_val[1])
        {
            //FTS_INFO("[UPGRADE]: erase app finished!!");
            break;
        }
        msleep(50);
    }

    /*write app to flash*/
    upgrade_ecc = 0;
    //FTS_INFO("[UPGRADE]: write app to flash!!");

    dw_lenth = fw_length;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = FTS_FW_WRITE_CMD;
    packet_buf[1] = 0x00;

    for (j = 0; j < packet_number; j++)
    {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (lenght >> 8);
        packet_buf[5] = (u8) lenght;

        for (i = 0; i < FTS_PACKET_LENGTH; i++)
        {
            packet_buf[6 + i] = pbt_buf[j * FTS_PACKET_LENGTH + i];
            upgrade_ecc ^= packet_buf[6 + i];
        }

        fts_i2c_write( packet_buf, FTS_PACKET_LENGTH + 6);

        for (i = 0; i < 30; i++)
        {
            auc_i2c_write_buf[0] = 0x6a;
            auc_i2c_write_buf[1] = 0x00;
            auc_i2c_write_buf[2] = 0x00;
            auc_i2c_write_buf[3] = 0x00;
            reg_val[0] = 0x00;
            reg_val[1] = 0x00;
            fts_i2c_read( auc_i2c_write_buf, 4, reg_val, 2);
            if (0xb0 == (reg_val[0] & 0xf0) && (0x03 + (j % 0x0ffd)) == (((reg_val[0] & 0x0f) << 8) |reg_val[1]))
            {
                //FTS_INFO("[UPGRADE]: write a block data finished!!");
                break;
            }
            //msleep(1);
            delay_us(1000);
        }
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0)
    {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (temp >> 8);
        packet_buf[5] = (u8) temp;

        for (i = 0; i < temp; i++)
        {
            packet_buf[6 + i] = pbt_buf[packet_number * FTS_PACKET_LENGTH + i];
            upgrade_ecc ^= packet_buf[6 + i];
        }

        fts_i2c_write( packet_buf, temp + 6);

        for (i = 0; i < 30; i++)
        {
            auc_i2c_write_buf[0] = 0x6a;
            auc_i2c_write_buf[1] = 0x00;
            auc_i2c_write_buf[2] = 0x00;
            auc_i2c_write_buf[3] = 0x00;
            reg_val[0] = 0x00;
            reg_val[1] = 0x00;
            fts_i2c_read( auc_i2c_write_buf, 4, reg_val, 2);
            if (0xb0 == (reg_val[0] & 0xf0) && (0x03 + (j % 0x0ffd)) == (((reg_val[0] & 0x0f) << 8) |reg_val[1]))
            {
                //FTS_INFO("[UPGRADE]: write a block data finished!!");
                break;
            }
            //msleep(1);
            delay_us(1000);
        }
    }

    /*read out checksum*/
    //FTS_INFO("[UPGRADE]: read out checksum!!");
    auc_i2c_write_buf[0] = FTS_REG_ECC;
    fts_i2c_read( auc_i2c_write_buf, 1, reg_val, 1);
    if (reg_val[0] != upgrade_ecc) /*check sum error, upgrade fail*/
    {
        //FTS_ERROR("[UPGRADE]: ecc error : FW=%02x upgrade_ecc=%02x!!",reg_val[0],upgrade_ecc);
        return -1;//-EIO;
    }
    //FTS_INFO("[UPGRADE]: ecc ok!!");

    /*upgrade success, reset the new FW*/
    //FTS_INFO("[UPGRADE]: reset the new FW!!");
    auc_i2c_write_buf[0] = 0x07;
    fts_i2c_write( auc_i2c_write_buf, 1);
    msleep(300);

    //FTS_FUNC_EXIT();

    return 0;
}

/*========================================================================================*/

static const u16 FT5336_TPX_TBL[5]={FT_TP1_REG,FT_TP2_REG,FT_TP3_REG,FT_TP4_REG,FT_TP5_REG};


//初始化FT5336触摸屏
//返回值:0,初始化成功;1,初始化失败
static BOOL FT5406_Init(void)
{
	u8 buf[32];

	FT5406_Init_Port();

// 	fts_ft6x36gu_upgrade_use_buf(ft6336_fw,sizeof(ft6336_fw));

#if 0
 	buf[0]=0;
	FT5406_WR_Reg(FT_DEVIDE_MODE,buf,1);	//进入正常操作模式
	SYS_msleep(50);

	FT5406_WR_Reg(FT_ID_G_MODE,buf,1);		//查询模式
	SYS_msleep(50);

	buf[0]=22;								//触摸有效值，22，越小越灵敏
	FT5406_WR_Reg(FT_ID_G_THGROUP,buf,1);	//设置触摸有效值
	SYS_msleep(50);

	buf[0]=12;								//激活周期，不能小于12，最大14
	FT5406_WR_Reg(FT_ID_G_PERIODACTIVE,buf,1);
	SYS_msleep(50);

	//读取版本号，参考值：0x3003
	FT5406_RD_Reg(FT_ID_G_LIB_VERSION,(u8*)&ver,2);

#if 0
	if((buf[0]==0X30&&buf[1]==0X03)||buf[1]==0X01||buf[1]==0X02)//版本:0X3003/0X0001/0X0002
	{
			printf("CTP ID:%x\r\n",((u16)buf[0]<<8)+buf[1]);
			return 0;
	}
#endif

 	FT5406_RD_Reg(FT_REG_NUM_FINGER,buf,1);   //读取触摸点的状态
 	FT5406_RD_Reg(FT5336_TPX_TBL[0],buf,24);  //读取XY坐标值


#endif
	return TRUE;
}

static int FT5406_GET_XY(int *x,int *y)
{
	u8 buf[16];

	if(!FT5406_RD_Reg(FT5336_TPX_TBL[0],buf,4))	//读取XY坐标值
	{
		return 0;
	}

	*x =((int)(buf[0]&0X0F)<<8)+buf[1];
	*y =((int)(buf[2]&0X0F)<<8)+buf[3];
	return 1;
}

static BOOL FT5406_Execu(int *px,int *py)
{
	int x,y,touch_num;
	u8 i;

	if(!FT5406_RD_Reg(FT_REG_NUM_FINGER,&i,1))//读取触摸点的状态
	{
		return FALSE;
	}

	if(i&0x80)
	{	//无效数据.
		return FALSE;
	}

	i &= 0x0F;
	touch_num =i;

	if(touch_num > 0)
	{
		if(touch_num <= MAX_TOUCH_NUM)
		{
			int x,y;

			if(FT5406_GET_XY(&x,&y) > 0)
			{
				*px =x;
				*py =y;
				 return TRUE;

			}
		}
	}

	return FALSE;

}

/*========================================================================================*/
/*========================================================================================*/


static void put_hex(WCHAR *wbuf,u8 *dat,int num)
{
	int i,a;

	a=0;
	for(i=0;i<num;i++)
	{
		x_wsprintf(&wbuf[a],L"%02X,",dat[i]);
		a =x_wstrlen(wbuf);
	}
}

#if 1
#include "x_libc.h"

u8 FT5406_Test(void)
{
	u8 buf[128];
	u8 i=0;
	u8 res=0;
	u8 temp;
	int x=0,y=0;
	int count=0;

	HDC hdc;
	RECT rc;


	FT5406_Init();


	while(1)
	{
#if 1
		i=0;
		FT5406_RD_Reg(FT_REG_NUM_FINGER,&i,1);//读取触摸点的状态
		if(i > 0)
		{
			//temp=0XFF<<(mode&0XF);//将点的个数转换为1的位数,匹配tp_dev.sta定义
			//tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES;
			//for(i=0;i<5;i++)
			{
				FT5406_RD_Reg(FT5336_TPX_TBL[0],buf,4);	//读取XY坐标值

				x=((u16)(buf[0]&0X0F)<<8)+buf[1];
				y=((u16)(buf[2]&0X0F)<<8)+buf[3];

			}

		}
#endif

		if(1)
		{

			WCHAR wbuf[512];
			u8 irq=0;
			u8 ack=0;

			rc.x =10;
			rc.y =20;
			rc.w =760;
			rc.h =32;

			//irq =GPIO_GetPin(GPX0,4);

			FT5406_RD_Reg(0x00,buf,32);	//读取XY坐标值

#if 1
			pIIC->Open(IIC_SPEED_100KHZ);
			pIIC->Start();
			if(pIIC->WriteByte(FT_CMD_WR,IIC_ACK))
			{
				ack=1;
			}
			else
			{
				ack=0;
			}
			pIIC->Stop();
			pIIC->Close();
			SYS_msleep(50);
#endif


			//FT5406_RD_Reg(0x00,buf,32);	//读取XY坐标值
			put_hex(wbuf,buf,32);

			hdc =GetDC(NULL);

			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);
			////
			OffsetRect(&rc,0,rc.h+4);
			x_wsprintf(wbuf,L"count:%d,irq=%d,ack=%d,hard=%d,ver:%04XH,i:%02XH,x:%d,y:%d ",count,irq,ack,hard_ok,ver,i,x,y);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);

			ReleaseDC(NULL,hdc);
			count++;


		}
		SYS_msleep(200);

	}


	return res;
}
#endif

/*========================================================================================*/
/*============================================================================*/

static int ts_x=0,ts_y=0;
static int ts_down=0;

static volatile POINT ts_pt;

static int GetState(void)
{
	int x,y;
	if(FT5406_Execu(&x,&y))
	{
		ts_pt.x =x;
		ts_pt.y =y;


		ts_x =x;
		ts_y =y;
		ts_down =1;
		return TRUE;
	}
	ts_down=0;
	return FALSE;


}

static BOOL GetSample(POINT *pt)
{
	if(ts_down==1)
	{
		pt->x =ts_x;
		pt->y =ts_y;
		return TRUE;
	}
	return FALSE;
}

static BOOL GetPoint(POINT *pt)
{
	if(GetSample(pt))
	{
		//TouchPanel_TranslatePoint(LCD_XSIZE,LCD_YSIZE,pt);
		return TRUE;
	}
	return	TRUE;
}



static BOOL LoadCfg(TS_CFG_DATA *cfg)
{

	return TRUE;
}

static BOOL SaveCfg(TS_CFG_DATA *cfg)
{
	return TRUE;
}

/*============================================================================*/

const GUI_TOUCH_DEV TouchDev_FT54xx=
{
	.Init 		=FT5406_Init,
	.GetState 	=GetState,
	.GetSample	=GetSample,
	.GetPoint	=GetPoint,
	.LoadCfg	=LoadCfg,
	.SaveCfg	=SaveCfg,
};

/*============================================================================*/
