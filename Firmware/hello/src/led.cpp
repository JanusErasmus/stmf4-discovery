#include <cyg/kernel/diag.h>
#include <cyg/hal/var_io.h>
#include <stdio.h>

#include "led.h"

cLED* cLED::__instance = NULL;

void cLED::init(ledPins_s* pinNumbers, cyg_uint8 pinCount)
{
	diag_printf("Initializing cLED\n");
	if(!__instance)
	{
		__instance = new cLED(pinNumbers, pinCount);
	}
}

cLED* cLED::get()
{
	return __instance;
}

cLED::cLED(ledPins_s* pinSpecs, cyg_uint8 pinCount) : mLEDCnt(pinCount)
{
	mLEDList = new ledPins_s[mLEDCnt];
	memcpy(mLEDList, pinSpecs, sizeof(ledPins_s) * mLEDCnt);

	setupPorts(mLEDList, mLEDCnt);

	animateTest();

	cyg_thread_create(LED_PRIORITY,
			cLED::led_thread,
			(cyg_addrword_t)this,
			(char *)"heartBeat",
			mLEDStack,
			LED_STACK_SIZE,
			&mLEDThreadHandle,
			&mLEDThread);
	cyg_thread_resume(mLEDThreadHandle);

}

void cLED::setupPorts(ledPins_s* ports, cyg_uint8 count)
{
	for (int k = 0; k < count; k++)
	{
		CYGHWR_HAL_STM32_GPIO_SET (ports[k].greenPinSpec);
		CYGHWR_HAL_STM32_GPIO_SET (ports[k].redPinSpec);
	}
}

void cLED::led_thread(cyg_addrword_t args)
{
	cLED* _this = (cLED*)args;

	for(;;)
	{
		_this->animateIdle();
	}
}

void cLED::animateIdle()
{
	eLEDcolor c1, c2;

	if(1)
	{
		c1 = red;
		c2 = green;
	}

	setLED(0, c1);
	setLED(1, c1);
	cyg_thread_delay(1);
	setLED(0, c2);
	setLED(1, c2);
	cyg_thread_delay(1);
	setLED(0, c1);
	setLED(1, c1);
	cyg_thread_delay(1);
	setLED(0, c2);
	setLED(1, c2);
	cyg_thread_delay(1);
	setLED(0, off);
	setLED(1, off);
	cyg_thread_delay(1);
}

void cLED::animateTest()
{
	for (int k = 0; k < mLEDCnt; k++)
		setLED(k,red);

	cyg_thread_delay(100);

	for (int k = 0; k < mLEDCnt; k++)
		setLED(k,green);

	cyg_thread_delay(100);
}

void cLED::setLED(cyg_uint8 led, eLEDcolor color)
{
	if(led >= mLEDCnt)
		return;

	cyg_uint8 state = 0;
	if(color == green)
		state = 1;

	switch(color)
	{
	case green:
		if(mLEDList[led].greenPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].greenPinSpec, 1);

		if(mLEDList[led].redPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].redPinSpec, 0);
		break;

	case red:
		if(mLEDList[led].greenPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].greenPinSpec, 0);

		if(mLEDList[led].redPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].redPinSpec, 1);
		break;

	case off:
		if(mLEDList[led].greenPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].greenPinSpec, 0);

		if(mLEDList[led].redPinSpec != 0xFFFFFFFF)
			CYGHWR_HAL_STM32_GPIO_OUT (mLEDList[led].redPinSpec, 0);
		break;
	}

}
