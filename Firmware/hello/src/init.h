#ifndef _INIT_H_
#define _INIT_H_
#include <cyg/kernel/kapi.h>


#include "definitions.h"
class cInit
{
	static cInit * __instance;

	cyg_uint8 mStack[INIT_STACK_SIZE];
	cyg_thread mThread;
	cyg_handle_t mThreadHandle;
	static void init_thread_func(cyg_addrword_t arg);


	cInit();

public:
	static void init();


};

#endif //Include Guard

