#include <cyg/kernel/diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/var_io.h>
#include <stdlib.h>

#include "definitions.h"
#include "uart_term.h"
#include "utils.h"
#include "init.h"
#include "led.h"
#include "F4_RTC.h"
#include "dog_LCD.h"
#include "usb_term.h"

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

    mPDx_IntHandle = 0;
}
/**
 * The main thread function for the system. The whole show
 * gets started from this.
 *
 * @param arg    Pointer to the instance of the cInit class
 */
void cInit::init_thread_func(cyg_addrword_t arg)
{

	CYGHWR_HAL_STM32_GPIO_SET(CYGHWR_HAL_STM32_GPIO(D, 5, GPIO_IN, 0, OPENDRAIN, NONE, 2MHZ));


	if(!F4RTC::init())
		diag_printf(RED("RTC NOT initialized\n"));

	cLED::ledPins_s ledPinNumbers[] = //no pin is 0xFF
	{
			{ CYGHWR_HAL_STM32_GPIO(D, 13, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ)},
			{ CYGHWR_HAL_STM32_GPIO(D, 12, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ)},
			{ CYGHWR_HAL_STM32_GPIO(D, 15, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ)},
			{ CYGHWR_HAL_STM32_GPIO(D, 14, GPIO_OUT, 0, PUSHPULL, NONE, 2MHZ)},
	};
	cLED::init(ledPinNumbers, 4);
//
//	dogLCD lcd;
//	lcd << "Hello";
//
	// Initialize the Terminal
	uartTerm::init("/dev/tty1",128,"discRTC>>");
	usbTerm::init(128, "discRTC>>");
//
//	//initButton();
//
//	char string[16];
//
//	lcd.setLine(2);
//	time_t now = time(NULL);
//	strftime(string, 16, "%a %m-%d-%Y", localtime(&now));
//	lcd << string;
//
//	lcd.setLine(3);
//	lcd << "Line 3";
//
//	lcd.setLine(4);
//		lcd << "Line 4";


	for (;;)
	{
		cyg_thread_delay(100);
//		lcd.setLine(1);
//		lcd << "Time ";
//		now = time(NULL);
//		strftime(string, 16, "%H:%M:%S", localtime(&now));
//		lcd << string;


	}
}

void cInit::initButton()
{
	cyg_interrupt_mask(CYGNUM_HAL_INTERRUPT_EXTI0);
	cyg_interrupt_create(CYGNUM_HAL_INTERRUPT_EXTI0,
			7,
			(cyg_addrword_t)0,
			handleISR,
			handleDSR,
			&mPDx_IntHandle,
			&mPDx_Interrupt);

	cyg_interrupt_attach(mPDx_IntHandle);
	cyg_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EXTI0);
}
