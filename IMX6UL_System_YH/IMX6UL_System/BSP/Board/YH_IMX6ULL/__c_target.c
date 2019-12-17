/*
 * retarget.c
 *
 *  Created on: 2017年6月12日
 *      Author: liuwei
 */

#include <stdio.h>
#include "Include.h"

/*=========================================================================================*/

int _write(FILE *fp)
{
	return 1;
}

int _read(FILE *fp)
{
	return 1;
}

int _fstat(FILE *fp)
{
	return 0;
}

int _lseek(FILE *fp)
{
	return 0;
}

void _close(FILE *fp)
{

}

int _isatty(FILE *fp)
{
	return 1;
}

/*=========================================================================================*/

int printf(const char *fmt,...)
{
#if 1
	char buf[200];
	x_va_list va;

	x_va_start(va,fmt);
	x_vsprintf(buf,fmt,va);
	x_va_end(va);

	//DebugPuts(buf);    // 需要实现？
	return x_strlen(buf);
#endif

}


/*=========================================================================================*/

void* malloc(size_t size)
{
	void *p;
	p= vmalloc(size);

	return p;
}

void free(void *ptr)
{
	vfree(ptr);
}

/*=========================================================================================*/
static u8 sbrk_buf[2048];

void* _sbrk(u32 size)
{
	volatile int i=0;
	//while(1)
	{
		i++;
	}
	return (void*)sbrk_buf;
}

/*=========================================================================================*/

int _getpid(void)
{
	return 0;
}

void _kill(void)
{

}

void _exit(void)
{

}

/*=========================================================================================*/

static	int	code_page=936;

void	x_set_codepage(int cp)
{
	code_page =cp;
}

/*============================================================================*/

typedef	struct
{
	int cp;
	int (*fun)(WCHAR *wchar,const char *mbchar,int count);
}fun_mbstowcs_tbl_t;

typedef	struct
{
	int cp;
	int (*fun)(char *mbchar,const WCHAR *wchar,int count);
}fun_wcstombs_tbl_t;


const	static	fun_mbstowcs_tbl_t mbstowcs_tbl[]={

#ifdef	CP437
	437,	x_mbstowcs_cp437,
#endif

#ifdef	CP720
	720,	x_mbstowcs_cp720,
#endif

#ifdef	CP737
	737,	x_mbstowcs_cp737,
#endif

#ifdef	CP775
	775,	x_mbstowcs_cp775,
#endif

#ifdef	CP850
	850,	x_mbstowcs_cp850,
#endif

#ifdef	CP852
	852,	x_mbstowcs_cp852,
#endif

#ifdef	CP855
	855,	x_mbstowcs_cp855,
#endif

#ifdef	CP858
	858,	x_mbstowcs_cp858,
#endif

#ifdef	CP862
	862,	x_mbstowcs_cp862,
#endif

#ifdef	CP866
	866,	x_mbstowcs_cp866,
#endif

#ifdef	CP874
	874,	x_mbstowcs_cp874,
#endif

#ifdef	CP1250
	1250,	x_mbstowcs_cp1250,
#endif

#ifdef	CP1251
	1251,	x_mbstowcs_cp1251,
#endif

#ifdef	CP1252
	1252,	x_mbstowcs_cp1252,
#endif

#ifdef	CP1253
	1253,	x_mbstowcs_cp1253,
#endif

#ifdef	CP1254
	1254,	x_mbstowcs_cp1254,
#endif

#ifdef	CP1255
	1255,	x_mbstowcs_cp1255,
#endif

#ifdef	CP1256
	1256,	x_mbstowcs_cp1256,
#endif

#ifdef	CP1257
	1257,	x_mbstowcs_cp1257,
#endif

#ifdef	CP1258
	1258,	x_mbstowcs_cp1258,
#endif

#ifdef	CP932
	932,	x_mbstowcs_cp932,
#endif

#ifdef	CP936
	936,	x_mbstowcs_cp936,
#endif

#ifdef	CP949
	949,	x_mbstowcs_cp949,
#endif

#ifdef	CP950
	950,	x_mbstowcs_cp950,
#endif

	0,		NULL

};
const	static	fun_wcstombs_tbl_t wcstombs_tbl[]={

#ifdef	CP437
	437,	x_wcstombs_cp437,
#endif

#ifdef	CP720
	720,	x_wcstombs_cp720,
#endif

#ifdef	CP737
	737,	x_wcstombs_cp737,
#endif

#ifdef	CP775
	775,	x_wcstombs_cp775,
#endif

#ifdef	CP850
	850,	x_wcstombs_cp850,
#endif

#ifdef	CP852
	852,	x_wcstombs_cp852,
#endif

#ifdef	CP855
	855,	x_wcstombs_cp855,
#endif

#ifdef	CP858
	858,	x_wcstombs_cp858,
#endif

#ifdef	CP862
	862,	x_wcstombs_cp862,
#endif

#ifdef	CP866
	866,	x_wcstombs_cp866,
#endif

#ifdef	CP874
	874,	x_wcstombs_cp874,
#endif

#ifdef	CP1250
	1250,	x_wcstombs_cp1250,
#endif

#ifdef	CP1251
	1251,	x_wcstombs_cp1251,
#endif

#ifdef	CP1252
	1252,	x_wcstombs_cp1252,
#endif

#ifdef	CP1253
	1253,	x_wcstombs_cp1253,
#endif

#ifdef	CP1254
	1254,	x_wcstombs_cp1254,
#endif

#ifdef	CP1255
	1255,	x_wcstombs_cp1255,
#endif

#ifdef	CP1256
	1256,	x_wcstombs_cp1256,
#endif

#ifdef	CP1257
	1257,	x_wcstombs_cp1257,
#endif

#ifdef	CP1258
	1258,	x_wcstombs_cp1258,
#endif

#ifdef	CP932
	932,	x_wcstombs_cp932,
#endif

#ifdef	CP936
	936,	x_wcstombs_cp936,
#endif

#ifdef	CP949
	949,	x_wcstombs_cp949,
#endif

#ifdef	CP950
	950,	x_wcstombs_cp950,
#endif

	0,		NULL

};


int	x_mbstowcs(WCHAR *wchar, const char *mbchar, int count)
{
   const fun_mbstowcs_tbl_t *p;

   p	=mbstowcs_tbl;

   while(p->fun!=NULL)
   {
   		if(p->cp == code_page)
   		{
   			return	p->fun(wchar,mbchar,count);
   		}
   		p++;

   }

 	return	x_mbstowcs_cp437(wchar,mbchar,count);
}

int	x_wcstombs(char *mbchar, const WCHAR *wchar, int count)
{
   const fun_wcstombs_tbl_t *p;

   p	=wcstombs_tbl;

   while(p->fun!=NULL)
   {
   		if(p->cp == code_page)
   		{
   			return	p->fun(mbchar,wchar,count);
   		}
   		p++;

   }

 	return	x_wcstombs_cp437(mbchar,wchar,count);

}

/*=========================================================================================*/
