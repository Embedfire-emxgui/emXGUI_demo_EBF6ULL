

#include	"Include.h"
#include	"ADS1120.h"

/*============================================================================*/
/*============================================================================*/


#define	ALCO_DEV_BUZ_ON					0x2000
#define	ALCO_DEV_SENSOR_HEAT_ON			0x2001
#define	ALCO_DEV_CANNULA_HEAT_ON		0x2002
#define	ALCO_DEV_AIR_PUMP_ON			0x2003
#define	ALCO_DEV_SENSOR_ENABLE			0x2004
#define	ALCO_DEV_SENSOR_PUMP_ON			0x2005
#define	ALCO_DEV_GET_FLOW_DATA			0x3000
#define	ALCO_DEV_GET_ALCO_DATA			0x3001
#define	ALCO_DEV_GET_INTERNAL_TEMP		0x3002
#define	ALCO_DEV_GET_EXTERNAL_TEMP		0x3003
#define	ALCO_DEV_OPEN_PRINTER			0x4000
#define	ALCO_DEV_CLOSE_PRINTER			0x4001
#define	ALCO_DEV_PRINT_OUTPUT			0x4002
#define	ALCO_DEV_BKLIGHT_ON				0x4003
#define	ALCO_DEV_GET_SN					0x4004
#define	ALCO_DEV_ABORT_PRINT			0x4005
#define	ALCO_DEV_SET_CODEPAGE			0x4006
#define	ALCO_DEV_GET_GPS_PORT			0x4007
#define	ALCO_DEV_SET_PRINTER_BYTE_DELAY	0x4008
#define	ALCO_DEV_SET_PRINTER_BAUD		0x4009
#define	ALCO_DEV_GET_ALCO_DATA_EX		0x4010
#define	ALCO_DEV_GET_COIN_COUNT			0x5000
#define	ALCO_DEV_SET_COIN_COUNT			0x5001

struct __get_alco_data_ex
{ //for ALCO_DEV_GET_ALCO_DATA_EX
	u16 *pBuf;
	u16 count;
	u16 time;
};

/*============================================================================*/
/*
	Buzzer      - GPB2 - Output(0;Off; 1:On)
	SEN_EN      - GPF6 - Output(0:Disable; 1:Enable)
	SEN_Pump    - GPF5 - Output(0:Off; 1:On)
	ALCO_PWR_EN - GPF7 - Output(0:Off; 1:On)
	Printer_PWR_EN - GPH8 - Output(0:Off; 1:On)
	
*/

#define	BUZ_ON()			//gpio->GPBDAT |=  (1<<2) //GPB2
#define	BUZ_OFF()			//gpio->GPBDAT &= ~(1<<2)

#define	SENSOR_ENABLE()		//gpio->GPFDAT |=  (1<<6) //GPF6
#define	SENSOR_DISABLE()	//gpio->GPFDAT &= ~(1<<6)

#define	SENSOR_PUMP_ON()	//gpio->GPFDAT |=  (1<<5) //GPF5
#define	SENSOR_PUMP_OFF()	//gpio->GPFDAT &= ~(1<<5)

#define	ALCO_MODULE_POWER_ON()	//gpio->GPFDAT |=  (1<<7) //GPF7
#define	ALCO_MODULE_POWER_OFF()	//gpio->GPFDAT &= ~(1<<7)

#define	SENSOR_PUMP_POWER_ON()	
#define	SENSOR_PUMP_POWER_OFF()	

#define	SENSOR_HEAT_ON()
#define	SENSOR_HEAT_OFF()

#define	CANNULA_HEAT_ON()	
#define	CANNULA_HEAT_OFF()

#define	AIR_PUMP_ON()
#define	AIR_PUMP_OFF()

#define	PRINTER_POWER_ON()  //gpio->GPHDAT |=  (1<<8) //GPH8
#define	PRINTER_POWER_OFF() //gpio->GPHDAT &= ~(1<<8)

#define	GPS_POWER_ON()		
#define	GPS_POWER_OFF()	

/*============================================================================*/
//static GPIO_REG *gpio	=GPIO_REG_VA_BASE;

static	int code_page=437;
static  u32 printer_baud=9600;
static	int printer_byte_delay=5000;

static volatile int coin_count=0;

/*============================================================================*/

static	void	buz_on(void)
{
	BUZ_ON();
}

static	void	buz_off(void)
{
	BUZ_OFF();
}

/*============================================================================*/

static	void	sensor_heat_on(void)
{
	SENSOR_HEAT_ON();
}

static	void	sensor_heat_off(void)
{
	SENSOR_HEAT_OFF();
}

/*============================================================================*/

static	void	cannula_heat_on(void)
{
	CANNULA_HEAT_ON();
}


static	void	cannula_heat_off(void)
{
	CANNULA_HEAT_OFF();
}

/*============================================================================*/

static	void	air_pump_on(void)
{
	AIR_PUMP_ON();
}

static	void	air_pump_off(void)
{
	AIR_PUMP_OFF();
}

/*============================================================================*/

static	void	sensor_enable(void)
{
	SENSOR_ENABLE();
}

static	void	sensor_disable(void)
{
	SENSOR_DISABLE();
}

/*============================================================================*/

static	void	alco_module_power_on(void)
{
	ALCO_MODULE_POWER_ON();
}

static	void	alco_module_power_off(void)
{
	ALCO_MODULE_POWER_OFF();
}

/*============================================================================*/

static	void	sensor_pump_power_on(void)
{
	SENSOR_PUMP_POWER_ON();
}

static	void	sensor_pump_power_off(void)
{
	SENSOR_PUMP_POWER_OFF();
}

/*============================================================================*/

static	void	sensor_pump_on(void)
{
	SENSOR_PUMP_ON();
}

static	void	sensor_pump_off(void)
{

	SENSOR_PUMP_OFF();
}

/*============================================================================*/

static	void	printer_power_on(void)
{
	PRINTER_POWER_ON();
}

static	void	printer_power_off(void)
{

	PRINTER_POWER_OFF();
}

/*============================================================================*/

static	void	gps_power_on(void)
{
	GPS_POWER_ON();		
}

static	void	gps_power_off(void)
{
	GPS_POWER_OFF();		
}

/*============================================================================*/


/*============================================================================*/

static	void	get_sn(U32 *sn)
{
	X_FILE *fp;
	U8 buf[32];
	char str[40];
	int i;
	
	i=0;
	if(sn)
	{
		fp	=x_fopen("B:sn.dat","rw");
		if(fp)
		{
			x_fseek(fp,4,SEEK_SET);
			
			i=x_fread(buf,8,1,fp);
			x_fclose(fp);
			
			if(i==8)
			{
				sn[0]	=buf[0]<<24 | buf[1]<<16 | buf[2]<<8 | buf[3]<<0;
				sn[1]	=buf[4]<<24 | buf[5]<<16 | buf[6]<<8 | buf[7]<<0;
		
			}
		//	x_sprintf(str,"sn:%08X-%08X\r\n" ,sn[0],sn[1]);
		//	DebugPuts(str);
							
			
		}
		
		if(i!=8)
		{
			sn[0]=0xFFFFFFFF;
			sn[1]=0xFFFFFFFF;	
		}
		
	}
}


/*============================================================================*/

static	void	Uart1_Init(void)
{
#if 0
	GPIO_REG	*gpio=GPIO_REG_VA_BASE;
	UART_REG	*uart=UART1_REG_VA_BASE;
	
 	gpio->GPHCON 	&= ~((3<<6)|(3<<4)) ;
	gpio->GPHCON 	|= ((2<<6)|(2<<4)) ;
   // gpio->GPHUDP  	=0x0;    // The pull up function is enable
	
	uart->UCON		=0x245;
  	uart->ULCON		=0x03;
    uart->UFCON 	=0x00;   //UART channel 1 FIFO control register, FIFO disable
    uart->UMCON 	=0x00;   //UART chaneel 1 MODEM control register, AFC disable
#endif

}

static	void Uart1_SetBaud(U32 baud)
{
#if 0
	int i;
	U32	PclkHZ;
	UART_REG	*uart=UART1_REG_VA_BASE;
	   
	////
	
	PclkHZ	=CPU_GetPclkHZ();		
	uart->UBRDIV	=(int)(PclkHZ/(baud*16))-1;
	uart->UDIVSLOT	=0x0000;
#endif

}

static	void	Uart1_Putch(char data)
{	
#if 0
 	UART_REG	*uart=UART1_REG_VA_BASE;

	while(!(uart->UTRSTAT & 0x2));	//Wait until THR is empty.
	uart->UTXH=data;
#endif
}

static	void	alco_dev_open_printer(void)
{
	Uart1_Init();
	Uart1_SetBaud(printer_baud);
	printer_power_on();
	Sleep(500);
	////
}

static	void	alco_dev_close_printer(void)
{
	printer_power_off();
}


static	void	alco_dev_print_output(char *buf,int size)
{
	volatile int a;
	int i;
	char ch;

	if(buf != NULL)
	{
		for(i=0;i<size;i++)
		{
			ch =buf[i];
			Uart1_Putch(ch);

			if(ch=='\n')
			{
				Sleep(100);
			}
		}
	}
}

static	void	alco_dev_abort_print(void)
{
	printer_power_off();
	Sleep(150);	
	printer_power_on();

}

/*============================================================================*/
/*============================================================================*/

static u32 ad_ch=0;
static u32 ad_pga=0;

static int ad_count=0; //需要的采样数量
static int ad_idx=0;   //当前采样数位置
static int ad_idx_0=0;


#define	MAX_AD_COUNT 100
static u16 ad_buf[MAX_AD_COUNT];

static u16 ad_buf0[10]; //AD临时缓冲区

static sem_obj *sem_alco_ad=NULL;
static mutex_obj *mutex_alco_ad=NULL;

void ALCO_Dev_OnTimer(void)
{

	int i,a;

	if(ad_count > 0)
	if(ad_idx < ad_count)
	{
		if(ad_idx_0 < 10)
		{
			a=ADS1120_GetData(ad_ch,ad_pga);
			ad_buf0[ad_idx_0]=a;
			ad_idx_0++;

			if(ad_idx_0 >= 10)
			{
				ad_idx_0=0;
				x_insert_sort_u16(ad_buf0,0,9);

				i=(ad_buf0[5] + ad_buf0[6])>>1;
				ad_buf[ad_idx] = i;

				ad_idx++;
				if(ad_idx >= ad_count)
				{
					SYS_sem_post(sem_alco_ad);
				}
			}
		}

	}
}
/*============================================================================*/

static	int	get_alco_data(void)
{
	int i=0;
	i=ADS1120_GetData(ADS1220_MUX_AIN0_AVSS,ADS1220_GAIN_128);
	return	i;
}


static void get_alco_data_ex(struct __get_alco_data_ex *p)
{
	int a;

	SYS_mutex_lock(mutex_alco_ad,0xFFFFFFFF);

	a =HAL_EnterCritical();
	ad_ch =ADS1220_MUX_AIN0_AVSS;
	ad_pga =ADS1220_GAIN_128;
	ad_idx_0 =0;
	ad_idx   =0;
	ad_count =MIN(p->count,MAX_AD_COUNT);
	HAL_ExitCritical(a);

	SYS_sem_wait(sem_alco_ad,5000);
	memcpy(p->pBuf,ad_buf,ad_count*2);

	SYS_mutex_unlock(mutex_alco_ad);

}

/*============================================================================*/

static	int	get_air_flow_data(void)
{
	int i,a;
	s16 ad;

	SYS_mutex_lock(mutex_alco_ad,0xFFFFFFFF);

	a =HAL_EnterCritical();
	ad_ch =ADS1220_MUX_AIN2_AIN3;
	ad_pga =ADS1220_GAIN_128;
	ad_idx_0 =0;
	ad_idx   =0;
	ad_count =1;
	HAL_ExitCritical(a);

	SYS_sem_wait(sem_alco_ad,5000);

	ad =(s16)ad_buf[0];
	SYS_mutex_unlock(mutex_alco_ad);

	if(ad < 0)
	{
		ad =0;
	}

	ad >>= 1;
	return	ad;

}

/*============================================================================*/

static	int	get_internal_temp_addata(void)
{
	int i,a;


	SYS_mutex_lock(mutex_alco_ad,0xFFFFFFFF);


	ADS1120_Config(	ADS1220_MUX_AIN1_AVSS,ADS1220_OP_MODE_NORMAL,
					ADS1220_CONVERSION_SINGLE_SHOT,ADS1220_DATA_RATE_1000SPS,ADS1220_GAIN_1,ADS1220_USE_PGA,
					ADS1220_IDAC1_AIN1,ADS1220_IDAC2_DISABLED,ADS1220_IDAC_CURRENT_100_UA);
	ADS1120_Start();
	ADS1220_WaitRdy();
	a=ADS1120_ReadData();
	Sleep(20);
	ADS1120_Start();
	ADS1220_WaitRdy();
	a=ADS1120_ReadData();

	SYS_mutex_unlock(mutex_alco_ad);

	return a;
}

/*============================================================================*/
#if 0  //电阻分压方式
#define	ADMAX	0x0FFF
#define	R1_REF	10000
#define	ADDATA(adc_max,R0,R1) (R1*adc_max)/(R1+R0)	//R0=上拉电阻
#endif

#if 1 //恒流源激励方式
#define	ImA			0.1 //0.1mA
#define ADMAX		0x8000
#define ADvref		2048 //2048mV
#define	ADDATA(TR) (U32)(ImA*TR*ADMAX)/ADvref
#endif

static const	U32	Temp_3435K_RefTab[]={

//TJD103  @25C=10K B=3435K
	
	ADDATA(0xFFFFFFFF),		//-20
	ADDATA(64562),				//-19
	ADDATA(61553),				//-18
	ADDATA(58700),				//-17
	ADDATA(55996),				//-16
	ADDATA(53432),				//-15
	ADDATA(50999),				//-14
	ADDATA(48692),				//-13
	ADDATA(46502),				//-12
	ADDATA(44423),				//-11
	ADDATA(42450),				//-10
	ADDATA(40568),				//-9
	ADDATA(38778),				//-8
	ADDATA(37077),				//-7
	ADDATA(35460),				//-6
	ADDATA(33922),				//-5
	ADDATA(32459),				//-4
	ADDATA(31067),				//-3
	ADDATA(29742),				//-2
	ADDATA(28481),				//-1
	
	ADDATA(27280),				//0
	ADDATA(26135),				//1
	ADDATA(25045),				//2
	ADDATA(24006),				//3
	ADDATA(23015),				//4
	ADDATA(22070),				//5
	ADDATA(21169),				//6
	ADDATA(20310),				//7
	ADDATA(19490),				//8
	ADDATA(18707),				//9
	
	ADDATA(17960),				//10
	ADDATA(17246),				//11
	ADDATA(16564),				//12
	ADDATA(15913),				//13
	ADDATA(15291),				//14
	ADDATA(14696),				//15
	ADDATA(14127),				//16
	ADDATA(13583),				//17
	ADDATA(13063),				//18
	ADDATA(12566),				//19

	ADDATA(12090),				//20
	ADDATA(11635),				//21
	ADDATA(11199),				//22
	ADDATA(10782),				//23
	ADDATA(10382),				//24
	ADDATA(10000),				//25
	ADDATA(9633),				//26
	ADDATA(9282),				//27
	ADDATA(8945),				//28
	ADDATA(8622),				//29

	ADDATA(8313),				//30
	ADDATA(8016),				//31
	ADDATA(7731),				//32
	ADDATA(7458),				//33
	ADDATA(7195),				//34
	ADDATA(6944),				//35
	ADDATA(6702),				//36
	ADDATA(6470),				//37
	ADDATA(6247),				//38
	ADDATA(6033),				//39
	
	ADDATA(5828),				//40
	ADDATA(5630),				//41
	ADDATA(5440),				//42
	ADDATA(5257),				//43
	ADDATA(5082),				//44
	ADDATA(4913),				//45
	ADDATA(4751),				//46
	ADDATA(4595),				//47
	ADDATA(4444),				//48
	ADDATA(4300),				//49
	
	ADDATA(4161),				//50
	ADDATA(4026),				//51
	ADDATA(3897),				//52
	ADDATA(3773),				//53
	ADDATA(3653),				//54
	ADDATA(3538),				//55
	ADDATA(3426),				//56
	ADDATA(3319),				//57
	ADDATA(3216),				//58
	ADDATA(3116),				//59
	
	ADDATA(3021),				//60
	ADDATA(2928),				//61
	ADDATA(2839),				//62
	ADDATA(2753),				//63
	ADDATA(2670),				//64
	ADDATA(2590),				//65
	ADDATA(2512),				//66
	ADDATA(2437),				//67
	ADDATA(2365),				//68
	ADDATA(2296),				//69
	
	ADDATA(2229),				//70
	ADDATA(2164),				//71
	ADDATA(2101),				//72
	ADDATA(2040),				//73
	ADDATA(1982),				//74
	ADDATA(1925),				//75
	ADDATA(1870),				//76
	ADDATA(1817),				//77
	ADDATA(1766),				//78
	ADDATA(1716),				//79
	
	//ADDATA(0000),				//80
	
	
	
};

////
static const	U32	Temp_3950K_RefTab[]={

	//MF5A  @25C=10K B=3950K
	
	ADDATA(0xFFFFFFFF),			//-20
	ADDATA(91660),				//-19
	ADDATA(86540),				//-18
	ADDATA(81720),				//-17
	ADDATA(77220),				//-16
	ADDATA(72980),				//-15
	ADDATA(69000),				//-14
	ADDATA(65260),				//-13
	ADDATA(61760),				//-12
	ADDATA(58460),				//-11
	ADDATA(55340),				//-10
	ADDATA(52420),				//-9
	ADDATA(49660),				//-8
	ADDATA(47080),				//-7
	ADDATA(44640),				//-6
	ADDATA(42340),				//-5
	ADDATA(40160),				//-4
	ADDATA(38120),				//-3
	ADDATA(36200),				//-2
	ADDATA(34380),				//-1
	
	ADDATA(32660),				//0
	ADDATA(31040),				//1
	ADDATA(29500),				//2
	ADDATA(28060),				//3
	ADDATA(26680),				//4
	ADDATA(25400),				//5
	ADDATA(24180),				//6
	ADDATA(23020),				//7
	ADDATA(21920),				//8
	ADDATA(20800),				//9
	
	ADDATA(19900),				//10
	ADDATA(18970),				//11
	ADDATA(18290),				//12
	ADDATA(17260),				//13
	ADDATA(16460),				//14
	ADDATA(15710),				//15
	ADDATA(15000),				//16
	ADDATA(14320),				//17
	ADDATA(13680),				//18
	ADDATA(13070),				//19

	ADDATA(12490),				//20
	ADDATA(11940),				//21
	ADDATA(11420),				//22
	ADDATA(10920),				//23
	ADDATA(10450),				//24
	ADDATA(10000),				//25
	ADDATA(9574),				//26
	ADDATA(9166),				//27
	ADDATA(8778),				//28
	ADDATA(8480),				//29

	ADDATA(8058),				//30
	ADDATA(7724),				//31
	ADDATA(7404),				//32
	ADDATA(7098),				//33
	ADDATA(6808),				//34
	ADDATA(6532),				//35
	ADDATA(6268),				//36
	ADDATA(6015),				//37
	ADDATA(5776),				//38
	ADDATA(5546),				//39
	
	ADDATA(5326),				//40
	ADDATA(5118),				//41
	ADDATA(4918),				//42
	ADDATA(4726),				//43
	ADDATA(4544),				//44
	ADDATA(4368),				//45
	ADDATA(4202),				//46
	ADDATA(4042),				//47
	ADDATA(3888),				//48
	ADDATA(3742),				//49
	
	ADDATA(3602),				//50
	ADDATA(3468),				//51
	ADDATA(3340),				//52
	ADDATA(3216),				//53
	ADDATA(3098),				//54
	ADDATA(2986),				//55
	ADDATA(2878),				//56
	ADDATA(2774),				//57
	ADDATA(2674),				//58
	ADDATA(2580),				//59
	
	ADDATA(2488),				//60
	ADDATA(2400),				//61
	ADDATA(2316),				//62
	ADDATA(2234),				//63
	ADDATA(2158),				//64
	ADDATA(2082),				//65
	ADDATA(2012),				//66
	ADDATA(1942),				//67
	ADDATA(1876),				//68
	ADDATA(1813),				//69
	
	ADDATA(1751),				//70
	ADDATA(1693),				//71
	ADDATA(1637),				//72
	ADDATA(1582),				//73
	ADDATA(1530),				//74
	ADDATA(1480),				//75
	ADDATA(1432),				//76
	ADDATA(1385),				//77
	ADDATA(1341),				//78
	ADDATA(1298),				//79
	
	//ADDATA(0000),				//80
	
	
	
};

static	int	get_internal_temp(void) //Sensor TEMP.
{
	int ad,i;
	////
#if 0
	if(ADC_Open())
	{
		ADC_Read(2);
		for(i=0;i<5000;i++);
		addata=ADC_Read(2);
		
		ADC_Close();
	}
#endif

	ad =get_internal_temp_addata();
	i =ARR_SIZE(Temp_3435K_RefTab)-1;
		
	while(i)
	{
		if(ad < Temp_3435K_RefTab[i])	break;
		i--;
	}
	////
	i +=- 20;
	return	i;

}

/*============================================================================*/

static	int	get_external_temp(void)
{
	int addata=0,i;
	////
#if 0
	if(ADC_Open())
	{
		ADC_SetChEx(1);
		ADC_Read(3);
		for(i=0;i<5000;i++);
		addata=ADC_Read(3);
		
		ADC_Close();
	}

	i	=ARR_SIZE(Temp_3950K_RefTab)-1;
		
	while(i)
	{
		if(addata<Temp_3950K_RefTab[i])	break;
		i--;
	}
	////
	i+=-20;
	return	i;
#endif
	return 25;
}

/*============================================================================*/

static int get_coin_count(void)
{
	int a,count;

	a=HAL_EnterCritical();
	count =coin_count;
	HAL_ExitCritical(a);
	return count;
}

static void set_coin_count(int count)
{
	int a;

	a=HAL_EnterCritical();
	coin_count =count;
	HAL_ExitCritical(a);
}

static void coin_isr(void)
{
	coin_count++;

	//EINT_ClearPend(11);
}
/*============================================================================*/

static	sys_err_t	alco_dev_create(dev_obj_t *dev,const void *create_data)
{

	/*
		Buzzer      - GPB2 - Output(0;Off; 1:On)
		SEN_EN      - GPF6 - Output(0:Disable; 1:Enable)
		SEN_Pump    - GPF5 - Output(0:Off; 1:On)
		ALCO_PWR_EN - GPF7 - Output(0:Off; 1:On)
		Printer_PWR_EN - GPH8 - Output(0:Off; 1:On)

	*/
	////

	sem_alco_ad =SYS_sem_create(0,1,NULL);
	mutex_alco_ad =SYS_mutex_create(NULL);

	ADS1120_Init();
#if 0
	GPIO_SetMode(GPB,2,GPIO_OUTPUT|GPIO_PULL_UP);	//BUZ_Drv
	GPIO_SetMode(GPF,6,GPIO_OUTPUT|GPIO_PULL_UP);	//SEN_EN
	GPIO_SetMode(GPF,5,GPIO_OUTPUT|GPIO_PULL_UP);	//SEN_Pump
	GPIO_SetMode(GPF,7,GPIO_OUTPUT|GPIO_PULL_UP);	//ALCO_PWR_EN
	GPIO_SetMode(GPH,8,GPIO_OUTPUT|GPIO_PULL_UP);	//Printer_PWR_EN
#endif

	
	code_page	=936;
	
	buz_off();
	sensor_heat_off();
	cannula_heat_off();
	air_pump_off();
	sensor_disable();
	sensor_pump_off();
	
	alco_module_power_off();
	sensor_pump_power_off();
	printer_power_off();
	gps_power_off();
	
	
	return SYS_NO_ERR;
}

/*============================================================================*/

static	sys_err_t	alco_dev_release(dev_obj_t *dev)
{

	return	SYS_NO_ERR;
}

/*============================================================================*/
static	sys_err_t	alco_dev_open(dev_open_t *open,dev_obj_t *dev)
{
	////
	buz_off();
	sensor_heat_off();
	cannula_heat_off();
	air_pump_off();
	sensor_disable();
	sensor_pump_off();

	alco_module_power_on();
	sensor_pump_power_on();
	printer_power_on();
	gps_power_on();

	////
	sensor_pump_on();
	Sleep(200);
	sensor_pump_off();
	code_page=936;
	printer_byte_delay=5000;
	printer_baud =9600;

	DebugPrintf("ALCO_Dev_Open.\r\n");

	return	SYS_NO_ERR;
}

static	sys_err_t	alco_dev_close(dev_open_t *open)
{
	
	buz_off();
	sensor_heat_off();
	cannula_heat_off();
	air_pump_off();
	sensor_disable();
	sensor_pump_off();
	
	alco_module_power_off();
	sensor_pump_power_off();
	printer_power_off();
	gps_power_off();

	DebugPrintf("ALCO_Dev_Close.\r\n");
	return SYS_NO_ERR;
}

static	int	alco_dev_ioctl(dev_open_t *open,int cmd,void *argv,int size)
{
	int	 a;
	a	=*(int*)argv;
	switch(cmd)
	{
		
		case	ALCO_DEV_BUZ_ON:
				if(a)
				{
					buz_on();
				}
				else
				{
					buz_off();
				}
				return	1;
				////
					
		case	ALCO_DEV_SENSOR_HEAT_ON:
				if(a)
				{
					sensor_heat_on();
				}
				else
				{
					sensor_heat_off();
				}
				return	1;
				////
				
		case	ALCO_DEV_CANNULA_HEAT_ON:
				if(a)
				{
					cannula_heat_on();
				}
				else
				{
					cannula_heat_off();
				}
				return 1;
				////
				
		case	ALCO_DEV_AIR_PUMP_ON:
				if(a)
				{
					air_pump_on();
				}
				else
				{
					air_pump_off();
				}
				return 1;
				////
				
		case	ALCO_DEV_SENSOR_ENABLE:
				if(a)
				{
					sensor_enable();
				}
				else
				{
					sensor_disable();
				}
				return	1;
				////
				
		case	ALCO_DEV_SENSOR_PUMP_ON:
				if(a)
				{
					sensor_pump_on();
				
				}
				else
				{
					sensor_pump_off();
				}
				return 1;
				////
				
		case	ALCO_DEV_GET_FLOW_DATA:
				a=	get_air_flow_data();
				*(int*)argv	=a;
				return 1;
				////
				
		case	ALCO_DEV_GET_ALCO_DATA:
				a=	get_alco_data();
				*(int*)argv	=a;
				return	1;
				////
				
		case	ALCO_DEV_GET_INTERNAL_TEMP:
				a=	get_internal_temp();
				*(int*)argv	=a;
				return	1;
				////
				
		case	ALCO_DEV_GET_EXTERNAL_TEMP:
				a=	get_external_temp();
				*(int*)argv	=a;
				return	1;
				////
					
		case	ALCO_DEV_GET_SN:				
				get_sn((void*)argv);
				return	TRUE;
				////
				
		case	ALCO_DEV_OPEN_PRINTER:
				alco_dev_open_printer();
				return	TRUE;
				////
				
		case	ALCO_DEV_CLOSE_PRINTER:
				alco_dev_close_printer();
				return	TRUE;
				////
				
		case	ALCO_DEV_PRINT_OUTPUT:
				alco_dev_print_output((char*)argv,size);
				return	TRUE;
				
		case	ALCO_DEV_SET_CODEPAGE:
				code_page	=*(int*)argv;
				return	TRUE;

				////
		case	ALCO_DEV_GET_GPS_PORT:
				if(argv)
				{		
					x_strcpy((char*)argv,"DEV:COM3");
					return	TRUE;
				}
				break;
				////
		case	ALCO_DEV_SET_PRINTER_BYTE_DELAY:
				printer_byte_delay =a;
				break;
				////

		case	ALCO_DEV_SET_PRINTER_BAUD:
		{
				printer_baud =*(u32*)argv;
		}
		return TRUE;
		////

		case    ALCO_DEV_GET_ALCO_DATA_EX:
		{
			struct __get_alco_data_ex *p;
			p =(struct __get_alco_data_ex*)argv;
			if(size == sizeof(struct __get_alco_data_ex))
			{
				get_alco_data_ex(p);
				return TRUE;
			}
			return FALSE;
		}
		////

		case	ALCO_DEV_GET_COIN_COUNT:
		{
			*(int*)argv =get_coin_count();
			return TRUE;
		}
		break;
		////
		case	ALCO_DEV_SET_COIN_COUNT:
		{
			set_coin_count(*(int*)argv);
			return TRUE;
		}
		break;
		////


		default:
		{
			return FALSE;
		}
				
	}

	return	0;
}
  
/*============================================================================*/
/*============================================================================*/

static dev_ops_t	ops;

void	ALCO_Register(void)
{

	
    memset(&ops,0,sizeof(dev_ops_t));

	
	ops.create	=alco_dev_create;
	ops.release	=alco_dev_release;
	ops.open	=alco_dev_open;
	ops.close	=alco_dev_close;
	ops.ioctl	=alco_dev_ioctl;
	ops.sync	=NULL;
	ops.write	=NULL;
	ops.read	=NULL;
	
	SYS_dev_create("ALCO_DEV",&ops,NULL);
	
}
/*============================================================================*/
/*============================================================================*/
