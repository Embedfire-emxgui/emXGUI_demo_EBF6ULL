
#ifndef	__N329XX_DEF_H__
#define	__N329XX_DEF_H__

////////////////////////////////
////常量定义

////CPSR
#define	USERMODE	    0x10
#define	FIQMODE			0x11
#define	IRQMODE		    0x12
#define	SVCMODE		    0x13
#define	ABORTMODE	    0x17
#define	UNDEFMODE	    0x1B
#define	SYSMODE		    0x1F
#define	MODEMASK	    0x1F
#define	NOINT		    0xC0

//// CP15 R1
#define	R1_M    (1<<0)
#define	R1_A	(1<<1)
#define	R1_C	(1<<2)
#define	R1_W	(1<<3)
#define	R1_Z	(1<<11)
#define	R1_I	(1<<12)
#define	R1_V	(1<<13)
#define	R1_RR	(1<<14)
#define	R1_VE	(1<<24)
#define	R1_nF   (1<<30)
#define	R1_iA	(1<<31)


#define	KB  (1024)
#define	MB	(1024*1024)

/*=========================================================================================*/
////参数配置





/////////////////////

#endif	//__N329XX_DEF_H__



