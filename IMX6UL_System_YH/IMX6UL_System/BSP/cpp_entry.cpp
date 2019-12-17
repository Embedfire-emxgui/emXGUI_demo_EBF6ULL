

#include "def.h"
#include "Kernel.h"
#include "emXGUI.h"
#include "x_libc.h"


int	GamepaperMain(HANDLE hInstance,void *argv);

extern "C" void CPP_Entry(void);
/*
UINT RandVal(void)
{
	return x_rand();
}*/

void CPP_Entry(void)
{
	//GamepaperMain(NULL,NULL);
}
