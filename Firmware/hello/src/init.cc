#include <cyg/kernel/diag.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/var_io.h>
#include <stdlib.h>

#include "definitions.h"
#include "term.h"

#include "init.h"
#include "led.h"

cInit * cInit::__instance = NULL;

/**
 * This function is the system init function.
 * It is conctructed in such a way as to be only
 * execute the constructor once, thus only initialising
 * the system once
 *
 * @example cInit::init();
 */
void cInit::init()
{
    if (__instance == NULL) //Allow instance to be creater only once.
    {
        __instance = new cInit();
    }
}
/**
 * The private default constructor.
 * The constructor is not callable from outside the class
 * and starts the show by creating the first system thread
 */
cInit::cInit()
{
    cyg_thread_create(INIT_PRIORITY,
                      cInit::init_thread_func,
                      NULL,
                      (char *)"Init Thread",
                      mStack,
                      INIT_STACK_SIZE,
                      &mThreadHandle,
                      &mThread);
    cyg_thread_resume(mThreadHandle);
}
/**
 * The main thread function for the system. The whole show
 * gets started from this.
 *
 * @param arg    Pointer to the instance of the cInit class
 */
void cInit::init_thread_func(cyg_addrword_t arg)
{

	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(AHB1, GPIOD));

	cLED::ledPins_s ledPinNumbers[] = //no pin is 0xFF
	{
			{ CYGHWR_HAL_STM32_GPIO(D, 13, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ), CYGHWR_HAL_STM32_GPIO(D, 12, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ) },
			{ CYGHWR_HAL_STM32_GPIO(D, 14, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ), CYGHWR_HAL_STM32_GPIO(D, 15, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ) },
	};

	cLED::init(ledPinNumbers, 2);


	// Initialize the Terminal
	cTerm::init((char *)"/dev/tty1",128,"disc>>");

	for (;;)
	{
		cyg_thread_delay(100);
	}
}

