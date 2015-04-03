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

#include "led.h"
#include "F4_RTC.h"
#include "input_port.h"
#include "keypad.h"

cInit * cInit::__instance = NULL;

cyg_uint32 sample()
{
	cyg_uint32 reg32;

	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, 0x00);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
			reg32 	|=
					CYGHWR_HAL_STM32_ADC_CR2_SWSTART |		//start conversion
					0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);

	do
		{
			HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, reg32);
		}
		while(!(reg32 & CYGHWR_HAL_STM32_ADC_SR_EOC));

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_DR, reg32);

	//diag_printf("ADC 0x%08X\n", reg32);
	return reg32;
}

cyg_uint32 adcBuff[32];
cyg_uint8 adcIdx = 0;

void setupTemperature()
{
	CYGHWR_HAL_STM32_GPIO_SET(TEMP_IN1);
	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(APB2, ADC1));

	cyg_thread_delay(100);
	cyg_uint32 reg32;

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SR, reg32);
	diag_printf("SR: 0x%08X\n", reg32);


//	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
	reg32 =
			CYGHWR_HAL_STM32_ADC_CR2_ADON |		//switch on
			0;
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);

	reg32 =
				CYGHWR_HAL_STM32_ADC_CR1_DISCEN |		//switch on
				0;
		HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR1, reg32);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR1, reg32);
	diag_printf("CR1: 0x%08X\n", reg32);

	HAL_READ_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_CR2, reg32);
	diag_printf("CR2: 0x%08X\n", reg32);


		reg32 = 1 |	//select channel 1 as 1st sequence
				0;
		HAL_WRITE_UINT32( CYGHWR_HAL_STM32_ADC1 + CYGHWR_HAL_STM32_ADC_SQR3, reg32);

		for(cyg_uint8 k = 0; k < 32; k++)
			adcBuff[k] = 0x989;

}



double getTemp(cyg_uint32 adc)
{
	static bool tempValid = false;
	adcBuff[adcIdx] = adc;
	//diag_printf("%d:0x%08X\n", adcIdx, adcBuff[adcIdx]);

	if(++adcIdx >= 32)
	{
		tempValid = true;
		adcIdx = 0;
	}


	cyg_uint32 adcSum = 0;
	for(cyg_uint8 k = 0; k < 32; k++)
	{
		adcSum += adcBuff[k];
	}

	float adcAvg = (float)(adcSum >> 5);

	if(tempValid)
		return (adcAvg * 0.122073125) - 273.0;

	return -1;
}

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
    mMainMenu = 0;
}


void cInit::init_thread_func(cyg_addrword_t arg)
{
	CYGHWR_HAL_STM32_GPIO_OUT(ELEMENT_OUTPUT,1);
	CYGHWR_HAL_STM32_GPIO_SET(ELEMENT_OUTPUT);
	CYGHWR_HAL_STM32_GPIO_SET(CYGHWR_HAL_STM32_PIN_IN(D, 5, FLOATING));


	if(!F4RTC::init())
		diag_printf(RED("RTC NOT initialized\n"));

//	cyg_uint32 ledPinNumbers[] = //no pin is 0xFF
//	{
//			CYGHWR_HAL_STM32_PIN_OUT(D, 13, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 12, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 15, PUSHPULL, NONE, 2MHZ),
//			CYGHWR_HAL_STM32_PIN_OUT(D, 14, PUSHPULL, NONE, 2MHZ),
//	};
//	cLED::init(ledPinNumbers, 4);

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



//	alphaNumericLCD::s_DataPins lcdPins(
//			LCD_RS,
//			LCD_RW,
//			LCD_E,
//			LCD_DB0,
//			LCD_DB1,
//			LCD_DB2,
//			LCD_DB3,
//			LCD_DB4,
//			LCD_DB5,
//			LCD_DB6,
//			LCD_DB7,
//			setDBpins
//	);
//	alphaNumericLCD * lcd = new alphaNumericLCD(lcdPins);

	setupTemperature();

//	__instance->mMainMenu = new cMainMenu(lcd, 0);
//	__instance->mMainMenu->open();

	// Initialize the Terminals
	char * prompt = "lcdMenu>>";
	//usbTerm::init(128, prompt);
	uartTerm::init("/dev/tty1", 128, prompt);

	cyg_uint8 dispCnt = 0;

	for (;;)
	{
		double temp = getTemp(sample());
		if(temp > 0)
		{
			if(dispCnt++ > 10)
			{
				dispCnt = 0;
			printf("T: %.1f\n", temp);
			}

			if(temp < 80.0)
				CYGHWR_HAL_STM32_GPIO_OUT(ELEMENT_OUTPUT, 1);

			if( temp > 85.0)
				CYGHWR_HAL_STM32_GPIO_OUT(ELEMENT_OUTPUT, 0);
		}

		cyg_thread_delay(10);
	}
}

void cInit::handleNavigation(cTerm & t,int argc,char *argv[])
{
	if(!__instance || !__instance->mMainMenu)
		return;

	if(!strcmp(argv[0], "u"))
	{
		__instance->mMainMenu->up();
	}

	if(!strcmp(argv[0], "d"))
	{
		__instance->mMainMenu->down();
	}

	if(!strcmp(argv[0], "e"))
	{
		__instance->mMainMenu->enter();
	}

	if(!strcmp(argv[0], "c"))
	{
		__instance->mMainMenu->cancel();
	}

	if(!strcmp(argv[0], "on"))
		{
		CYGHWR_HAL_STM32_GPIO_OUT(ELEMENT_OUTPUT, 1);
		}

	if(!strcmp(argv[0], "off"))
		{
		CYGHWR_HAL_STM32_GPIO_OUT(ELEMENT_OUTPUT, 0);
		}
}

