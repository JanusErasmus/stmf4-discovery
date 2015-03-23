#include <cyg/kernel/diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/var_io.h>
#include <stdlib.h>

#include <uart_term.h>
#include <usb_term.h>

#include "definitions.h"
#include "utils.h"
#include "init.h"

#include "anLCD.h"
#include "led.h"
#include "F4_RTC.h"
#include "input_port.h"

cInit * cInit::__instance = NULL;


void setDBpins(cyg_uint8 data)
{
	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
	reg32 &= (0xFFFF00FF);
	reg32 |= data << 8;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
}

void cInit::init()
{
    if (__instance == NULL) //Allow instance to be creater only once.
    {
        __instance = new cInit();
    }
}

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

void cInit::init_thread_func(cyg_addrword_t arg)
{

	CYGHWR_HAL_STM32_GPIO_SET(CYGHWR_HAL_STM32_PIN_IN(D, 5, FLOATING));


	if(!F4RTC::init())
		diag_printf(RED("RTC NOT initialized\n"));

	cyg_uint32 ledPinNumbers[] = //no pin is 0xFF
	{
			CYGHWR_HAL_STM32_PIN_OUT(D, 13, PUSHPULL, NONE, 2MHZ),
			CYGHWR_HAL_STM32_PIN_OUT(D, 12, PUSHPULL, NONE, 2MHZ),
			CYGHWR_HAL_STM32_PIN_OUT(D, 15, PUSHPULL, NONE, 2MHZ),
			CYGHWR_HAL_STM32_PIN_OUT(D, 14, PUSHPULL, NONE, 2MHZ),
	};
	cLED::init(ledPinNumbers, 4);


	alphaNumericLCD::s_DataPins lcdPins(
			LCD_RS,
			LCD_RW,
			LCD_E,
			LCD_DB0,
			LCD_DB1,
			LCD_DB2,
			LCD_DB3,
			LCD_DB4,
			LCD_DB5,
			LCD_DB6,
			LCD_DB7,
			setDBpins
			);

	alphaNumericLCD lcd(lcdPins);
	lcd << "Hello";

	// Initialize the Terminals
	char * prompt = "lcdMenu>>";
	usbTerm::init(128, prompt);
	uartTerm::init("/dev/tty1", 128, prompt);


	char string[16];

	lcd.setLine(2);
	time_t now = time(NULL);
	strftime(string, 16, "%a %m-%d-%Y", localtime(&now));
	lcd << string;

	lcd.setLine(3);
	lcd << "Line 3";

	lcd.setLine(4);
	lcd << "Line 4";

	cyg_uint32 portSpec[] = {
			CYGHWR_HAL_STM32_PIN_IN(A, 0, FLOATING),
	};
	cInput::init( portSpec, 1);
	cInput::get()->start();

	for (;;)
	{
		cyg_thread_delay(100);
		lcd.setLine(1);
		lcd << "Time ";
		now = time(NULL);
		strftime(string, 16, "%H:%M:%S", localtime(&now));
		lcd << string;
	}
}

