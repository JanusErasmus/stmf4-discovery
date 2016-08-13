#ifndef _INIT_H_
#define _INIT_H_
#include <cyg/kernel/kapi.h>
#include <time.h>
#include <term.h>

#include "definitions.h"
#include "menu_main.h"
#include "ht_monitor.h"
#include "environment.h"

class cInit
{
	static cInit * __instance;

	cEnvironment *mEnv;
	cHTmonitor * htSensor;
	cMainMenu * mMainMenu;
	time_t mRotateTime;

	cyg_uint8 mStack[INIT_STACK_SIZE];
	cyg_thread mThread;
	cyg_handle_t mThreadHandle;
	static void init_thread_func(cyg_addrword_t arg);

	cyg_interrupt mPDx_Interrupt;
	cyg_handle_t mPDx_IntHandle;

	cInit();
	void run();

	void initButton();
	void setupDisplay();
	void handleMotor();

public:
	static void init();

	static void forceRotate();
	static void handleNavigation(cTerm & t,int argc,char *argv[]);

	static void debug(cTerm & t,int argc,char *argv[]);
};

#endif //Include Guard

