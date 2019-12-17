
#include "Include.h"
#include "x_file.h"

//static APP_Execu(void *addr,)

void APP_Loader(void)
{
	void *p;
	X_FILE *fp;
	int fsize;

	char buf[128];

	if(!CDlg_FileExpoler(NULL,20,30,300,300,L"APP Loader","B:",buf))
	{
		return;
	}

	SYS_execu(buf,NULL,0);
	return;

}
