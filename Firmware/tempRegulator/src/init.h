#ifndef _INIT_H_
#define _INIT_H_
#include <cyg/kernel/kapi.h>

#include <term.h>

#include "definitions.h"
#include "menu_main.h"

class cInit
{
	static cInit * __instance;

	cMainMenu * mMainMenu;

	cyg_uint8 mStack[INIT_STACK_SIZE];
	cyg_thread mThread;
	cyg_handle_t mThreadHandle;
	static void init_thread_func(cyg_addrword_t arg);

	cyg_interrupt mPDx_Interrupt;
	cyg_handle_t mPDx_IntHandle;

	cInit();
	void initButton();

public:
	static void init();

	static void handleNavigation(cTerm & t,int argc,char *argv[]);


};

#endif //Include Guard

