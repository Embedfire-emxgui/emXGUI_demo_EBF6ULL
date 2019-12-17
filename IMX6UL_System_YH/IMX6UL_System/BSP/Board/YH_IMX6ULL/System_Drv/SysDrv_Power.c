
#include	"Include.h"

/*============================================================================*/

//GPH13
#define	PWR_ON()	//gpio->GPHDAT	|=  (1<<13);
#define	PWR_OFF()	//gpio->GPHDAT	&= ~(1<<13);

#define	BK_LT_ON()	//gpio->rGPBDAT	|=  (1<<2);
#define	BK_LT_OFF()	//gpio->rGPBDAT	&=~ (1<<2);


/*============================================================================*/

//static	GPIO_REG *gpio=GPIO_REG_VA_BASE;

/*============================================================================*/

static BOOL	_PowerON(void)
{

#ifdef	GPIO_SYS_PWR_EN
	gpio_pin_config_t gpio_cfg;

	gpio_cfg.direction =kGPIO_DigitalOutput;
	gpio_cfg.interruptMode =kGPIO_NoIntmode;
	gpio_cfg.outputLogic =1;


	IOMUXC_SetPinMux(IOMUXC_SYS_PWR_EN,0U);
	IOMUXC_SetPinConfig(IOMUXC_SYS_PWR_EN,PIN_CONFIG);

	GPIO_PinInit(GPIO_SYS_PWR_EN, PIN_SYS_PWR_EN, &gpio_cfg);
#endif

	PWR_ON();

	return TRUE;
	
}

/*============================================================================*/

static BOOL	_PowerOFF(void)
{
	PWR_OFF();
	return TRUE;
}

/*============================================================================*/

/*============================================================================*/
/*============================================================================*/
#define	BATT_ADC_CH 	0
#define	EXPWR_ADC_CH	1

static u16 batt_adbuf[16];
static u16 expwr_adbuf[16];

static volatile u16  batt_adval  =0x0FFF;
static volatile u16  expwr_adval =0x0000;

static int batt_adc_idx=0;
static int expwr_adc_idx=0;

u16	ADC_Read(int ch);

void SysDrv_Power_OnTimer100ms(void)
{
	static int step=0;
	volatile int i, a;

	if(step == 0)
	{
		TSADC_Lock();
		a = ADC_Read(BATT_ADC_CH);
		a = 0;
		a += ADC_Read(BATT_ADC_CH);
		a += ADC_Read(BATT_ADC_CH);
		a >>= 1;
		TSADC_Unlock();

		batt_adbuf[batt_adc_idx++] = a;
		if(batt_adc_idx >= 16)
		{
			x_insert_sort_u16(batt_adbuf,0,9);
			a=0;
			for(i=7;i<15;i++)
			{
				a += batt_adbuf[i];
			}
			a >>= 3;

			batt_adval =a;
			batt_adc_idx = 0;
			step =1;

		}

	}
	else
	{
		TSADC_Lock();
		a = ADC_Read(EXPWR_ADC_CH);
		a = 0;
		a += ADC_Read(EXPWR_ADC_CH);
		a += ADC_Read(EXPWR_ADC_CH);
		a >>= 1;
		TSADC_Unlock();

		expwr_adbuf[expwr_adc_idx++] = a;
		if(expwr_adc_idx >= 16)
		{
			x_insert_sort_u16(expwr_adbuf,0,9);
			a=0;
			for(i=7;i<15;i++)
			{
				a += expwr_adbuf[i];
			}
			a >>= 3;

			expwr_adval =a;
			expwr_adc_idx = 0;
			step =0;

		}
	}

}

/*============================================================================*/

#define	AD_MAX	0x0FFF
#define	AD_VREF	3300

//////////////////////////

#define	MIN_BAT_VOL		6800
#define	MAX_BAT_VOL		7600

#define	EX_PWR_CHK_R0	2000
#define	EX_PWR_CHK_R1	1000

#define	BATT_CHK_R0 	2000
#define	BATT_CHK_R1		1000

//////////////////////////

static u16 ad_to_voltage(u16 ad_range,u16 ad_vref,u16 r1,u16 r2,u16 ad)
{
	u32 v;

	v =ad*ad_vref/ad_range;
	v =v*(r1+r2)/r2;
	return v;
}

#if 1
static u32	get_batt_volatage(void)
{
	u32 vol;

	////
	vol =ad_to_voltage(AD_MAX,AD_VREF,BATT_CHK_R0,BATT_CHK_R1,batt_adval);
	return	vol;
}

static u32	get_expwr_volatage(void)
{
	u32 vol;

	////
	vol =ad_to_voltage(AD_MAX,AD_VREF,EX_PWR_CHK_R0,EX_PWR_CHK_R1,expwr_adval);
	return	vol;
}
#endif
/*============================================================================*/

BOOL	SysDrv_GetSystemPowerStatus(SYSTEM_POWER_STATUS* pwr)
{
	u32	batt_vol;
	u32 extpwr_vol;
	int	percent;
	////
#if 0
	ad	=batt_adval;
	batt_vol = (float)ad*((float)AD_VREF/(float)AD_MAX);
	batt_vol = batt_vol*(float)(BATT_CHK_R0+BATT_CHK_R1)/(float)BATT_CHK_R1;
#endif
	batt_vol =ad_to_voltage(AD_MAX,AD_VREF,BATT_CHK_R0,BATT_CHK_R1,batt_adval);

#if 0
	ad	=expwr_adval;
	extpwr_vol = (float)ad*((float)AD_VREF/(float)AD_MAX);
	extpwr_vol = extpwr_vol*(float)(EX_PWR_CHK_R0+EX_PWR_CHK_R1)/(float)EX_PWR_CHK_R1;
#endif
	extpwr_vol =ad_to_voltage(AD_MAX,AD_VREF,EX_PWR_CHK_R0,EX_PWR_CHK_R1,expwr_adval);

	percent	=(batt_vol-MIN_BAT_VOL)*100/(MAX_BAT_VOL-MIN_BAT_VOL);
	percent	=MAX(0,percent);
	percent	=MIN(100,percent);

	//percent =75;
	//batt_vol =5;

	////
	pwr->ExPowerFlag =POWER_FLAG_ACLINE;

	pwr->ExPowerLifePercent  =POWER_PERCENTAGE_UNKNOWN;
	pwr->ExPowerLifeTime     =POWER_LIFE_UNKNOWN;
	pwr->ExPowerFullLifeTime =POWER_LIFE_UNKNOWN;

	pwr->ExPowerVoltage      =extpwr_vol;
	pwr->ExPowerCurrent      =1000; //1000mA

	////
	pwr->BatteryFlag =0;
	if(extpwr_vol > 2000)
	{
		pwr->BatteryFlag |=POWER_FLAG_CHARGING;
	}

	if(batt_vol <= MIN_BAT_VOL)
	{
		pwr->BatteryFlag |=POWER_FLAG_LOW;
	}

	if(batt_vol >= MAX_BAT_VOL)
	{
		pwr->BatteryFlag |=POWER_FLAG_HIGH;
	}

	pwr->BatteryLifePercent  =percent;
	pwr->BatteryLifeTime     =POWER_LIFE_UNKNOWN;
	pwr->BatteryFullLifeTime =POWER_LIFE_UNKNOWN;

	pwr->BatteryVoltage      =batt_vol;
	pwr->BatteryCurrent      =500; //500mA
	return TRUE;

}

/*============================================================================*/

U32	SysDrv_SetSystemPowerState(U32 StateFlags)
{
	switch(StateFlags)
	{

		case	POWER_STATE_ON:
				_PowerON();
				break;
				////

		case	POWER_STATE_IDLE:

				break;
				////
		case	POWER_STATE_SUSPEND:

				break;
				////

		case	POWER_STATE_OFF:
				_PowerOFF();
				break;
				////

		default:
				break;
	}

	return FALSE;
}

/*============================================================================*/
