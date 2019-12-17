
#ifndef	__X_FILE_H__
#define	__X_FILE_H__

#ifdef	__cplusplus
extern	"C"{
#endif

#include "def.h"
#include "Kernel.h"

/*===================================================================================*/

#define SEEK_SET         0
#define SEEK_CUR         1
#define SEEK_END         2

typedef	struct	_X_FILE	X_FILE;

/*
   x_fopen-mode参数:
 [r]:   以只读方式打开文件，该文件必须存在。
 [r+]:  以可读写方式打开文件，该文件必须存在。
 [rb+]: 读写打开一个二进制文件，允许读写数据，文件必须存在。
 [w]:   打开只写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
 [w+]:  打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
 [wb+]: 以读/写方式打开或新建一个二进制文件，允许读和写。
 [a]:   以附加的方式打开只写文件。若文件不存在，则会创建该文件;如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留(EOF 符保留)。
 [a+]:  以附加方式打开可读/写的文件。若文件不存在，则会创建该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留(EOF符不保留)。
 [ab+]: 以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
 */
X_FILE* x_fopen(const char *filename, const char *mode);
long 	x_fread(void *buffer,long size, long count, X_FILE *fp);
long 	x_fwrite(const void *buffer,long size, long count, X_FILE *fp);

int 	x_fseek(X_FILE *fp, long offset, int whence);
unsigned long 	x_ftell(X_FILE *fp);
unsigned long 	x_fsize(X_FILE *fp);
int 	x_feof(X_FILE *fp);
int 	x_fflush(X_FILE *fp);
int 	x_fclose(X_FILE *fp);
int 	x_remove(const char *filename);

/*===================================================================================*/

typedef	enum {

	SCAN_EXIT =0,
	SCAN_NEXT,
	SCAN_DIR,

}eFILESCAN_CMD;


typedef	eFILESCAN_CMD	cbFileScan(const char *fpath,const char *fname,struct dir_info *d_info,const void* pdata);

typedef struct	scan_dir_data
{
	char	*path_buf;
	int		 path_buf_bytes;
	char	*fname_buf;
	int		 fname_buf_bytes;
	cbFileScan	*callback;
	const void	*pdata;
	struct dir_info d_info;

	//eFILESCAN_CMD	scan_cmd;

}scan_dir_data_t;

void x_file_scan(scan_dir_data_t *scan_data);

/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
