
#include	"Include.h"

/*============================================================================================*/

#define	DEF_YEAR	2011
#define	DEF_MONTH	1
#define	DEF_DAY		1
#define	DEF_HOUR	0
#define	DEF_MIN		0
#define	DEF_SEC		0

/*============================================================================================*/

BOOL SysDrv_SetLocalTime(const local_time_t *tm);
BOOL SysDrv_GetLocalTime(local_time_t *tm);

/*============================================================================================*/
#if 0
int	RTC_Init(void)
{		
		local_time_t	time;
		////	
		SysDrv_GetLocalTime(&time);
		
		if(time.year<2000)	goto	time_reset;
		if(time.year>2099)	goto	time_reset;
		
		if(time.mon<1)	goto	time_reset;
		if(time.mon>12)	goto	time_reset;
		
		if(time.day<1)		goto	time_reset;
		if(time.day>31)	goto	time_reset;
		
		if(time.hour>23)	goto	time_reset;
		if(time.min>59)	goto	time_reset;
		if(time.sec>59)	goto	time_reset;
							
		////	
		return TRUE;
			
time_reset:
		time.year	=DEF_YEAR;
		time.mon	=DEF_MONTH;
		time.day	=DEF_DAY;
		time.hour	=DEF_HOUR;
		time.min	=DEF_MIN;
		time.sec	=DEF_SEC;
		SysDrv_SetLocalTime(&time);
		return TRUE;
					
}
#endif
/*============================================================================================*/

BOOL SysDrv_SetLocalTime(const local_time_t *tm)
{

	return	TRUE;
}

/*============================================================================================*/

BOOL SysDrv_GetLocalTime(local_time_t *tm)
{
#if 0
	RTC_TIME_DATA_T t;

	RTC_Read(RTC_CURRENT_TIME,&t);

	tm->year = t.u32Year;
	tm->mon  = t.u32cMonth;
	tm->day  = t.u32cDay;
	tm->hour = t.u32cHour;
	tm->min  = t.u32cMinute;
	tm->sec  = t.u32cSecond;
#endif

#if 1
	u8 year,mon,day,hour,min,sec;
	
	RX8010_GetDate(&year,&mon,&day);
	RX8010_GetTime(&hour,&min,&sec);
	
	tm->year = year;
	tm->mon  = mon;
	tm->day  = day;
	tm->hour = hour;
	tm->min  = min;
	tm->sec  = sec;
	
#endif

#if 0
	tm->year = 0;
	tm->mon  = 1;
	tm->day  = 1;
	tm->hour = 0;
	tm->min  = 0;
	tm->sec  = 0;
#endif

	return	TRUE;
}


/*============================================================================================*/
/*============================================================================================*/
