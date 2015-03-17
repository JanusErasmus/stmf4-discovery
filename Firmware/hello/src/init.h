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

	cyg_interrupt mPDx_Interrupt;
	cyg_handle_t mPDx_IntHandle;
	static cyg_uint32 handleISR(cyg_vector_t vector,cyg_addrword_t data);
	static void handleDSR(cyg_vector_t vector,cyg_uint32 count,cyg_addrword_t data);

	cInit();
	void initButton();

public:
	static void init();


};

#endif //Include Guard

