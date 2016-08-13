#include <cyg/kernel/diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/var_io.h>
#include <stdlib.h>
#include <stdio.h>

#include <uart_term.h>
#include <usb_term.h>

#include "definitions.h"
#include "utils.h"
#include "init.h"

#include "debug_display.h"
#include "anLCD.h"
#include "menu_main.h"

#include "utils.h"
#include "led.h"
#include "F4_RTC.h"
#include "input_port.h"
#include "keypad.h"
#include "temp_probe.h"
#include "i2c_dev.h"
#include "ht_stmHTS221.h"
#include "output_port.h"
#include "motor.h"
#include "watchdog.h"
#include "nvm.h"

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
    cWatchDog::start();

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
    mMainMenu = 0;
    mRotateTime  = 0;
    htSensor = 0;
    mEnv = 0;
}

void cInit::init_thread_func(cyg_addrword_t arg)
{
	cNVM::init();

	cOutput* ports[] = {
			new cOutput(POWER_ENABLE),
			new cOutput(RL1_ON, true),
			new cOutput(RL2_ON, true),
			new cOutput(RL3_ON, true),
			new cOutput(RL4_ON, true),
			0
	};
	cOutputDriver::init(ports);

	ports[0]->set();

	cMotor::init(ports[2], ports[1]);

	__instance->mEnv = new cEnvironment(ports[4], ports[3]);

	if(!F4RTC::init())
		diag_printf("RTC NOT initialized\n");

//	cyg_uint32 ledPinNumbers[] = //no pin is 0xFF
//	{
//			CYGHWR_HAL_STM32_PIN_OUT(D, 13, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 12, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 15, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 14, PUSHPULL, NONE, 2MHZ),
//	};
//	cLED::init(ledPinNumbers, 4);

	__instance->setupDisplay();

	__instance->htSensor = new cHTstmHTS221(&HTS_humidity);
	cyg_bool htStatus = __instance->htSensor->getStatus();
	if(!htStatus)
	{
		delete __instance->htSensor;
		__instance->htSensor = 0;
		diag_printf(RED("HT MONITOR: NO SENSOR DETECTED\n"));
	}

	// Initialize the Terminals
	char * prompt = "lcdMenu>>";
	//usbTerm::init(128, prompt);
	uartTerm::init("/dev/tty1", 128, prompt);


	for (;;)
	{
		__instance->run();
	}
}

void cInit::run()
{

	if(htSensor)
	{
		analogSeq::seqSample temp = htSensor->getPortSample(cHTmonitor::TEMPERATURE);
		cyg_thread_delay(100);
		analogSeq::seqSample humid = htSensor->getPortSample(cHTmonitor::HUMIDITY);

//		double temp = probe->getTemp();

		static cyg_uint8 validSequence = 0;
		if(validSequence != temp.sequence)
		{
			diag_printf("sampled %d != %d\n", temp.sequence, validSequence);
			validSequence = temp.sequence;
			cWatchDog::kick();
		}

		cyg_bool heater = false, water = false;
		if(mEnv)
		{
			mEnv->run(temp.value, humid.value);
			mEnv->getStates(heater, water);
		}

		int rotation = __instance->mRotateTime + cNVM::getInterval() - time(0);
		if(rotation < 0)
			rotation = 0;

		mMainMenu->updateReading(temp.value, humid.value, heater, water, rotation);
	}

	handleMotor();
	cyg_thread_delay(500);

}

void cInit::setupDisplay()
{
	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(AHB1, GPIOC));

	cyg_uint32 columns[] =
	{
			KEYPAD_C1,
			KEYPAD_C2,
			KEYPAD_C3,
			KEYPAD_C4,
	};
	cyg_uint32 rows[] =
	{
			KEYPAD_R1,
			KEYPAD_R2,
			KEYPAD_R3,
			KEYPAD_R4,
	};

	cKeypad * pad = new cKeypad(columns, 4, rows, 4);

	cyg_uint32 portSpec[] = {
			CYGHWR_HAL_STM32_PIN_IN(A, 0, NONE),
			KEYPAD_C1,
			KEYPAD_C2,
			KEYPAD_C3,
			KEYPAD_C4,
	};
	cInput::init(portSpec,5);


	cInput::get()->setListener(1, pad);
	cInput::get()->setListener(2, pad);
	cInput::get()->setListener(3, pad);
	cInput::get()->setListener(4, pad);
	cInput::get()->start();

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

	alphaNumericLCD * lcd = new alphaNumericLCD(lcdPins);

	__instance->mMainMenu = new cMainMenu(lcd, 0);
	pad->setMenu(__instance->mMainMenu);
	__instance->mMainMenu->open();
}

void cInit::handleNavigation(cTerm & t,int argc,char *argv[])
{
	if(!__instance || !__instance->mMainMenu)
		return;

	if(!strcmp(argv[0], "on"))
	{
		CYGHWR_HAL_STM32_GPIO_OUT(POWER_ENABLE, 1);
	}

	if(!strcmp(argv[0], "off"))
	{
		CYGHWR_HAL_STM32_GPIO_OUT(POWER_ENABLE, 0);

	}
}

void cInit::handleMotor()
{
	if((mRotateTime + cNVM::getInterval()) < (cyg_uint32)time(0))
	{
		__instance->mRotateTime = time(0);
		cMotor::get()->rotate();
	}
}

void cInit::debug(cTerm & t,int argc,char *argv[])
{
	if(!__instance)
		return;

	diag_printf("Next rotation will be in %ds\n", (cyg_uint32)(__instance->mRotateTime + cNVM::getInterval() - time(0)));
}

void cInit::forceRotate()
{
	__instance->mRotateTime = 0;
};
