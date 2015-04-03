#include <cyg/kernel/diag.h>
#include <cyg/hal/var_io.h>
#include <cyg/kernel/kapi.h>
#include <ctype.h>
#include <string.h>
#include <cyg/io/ttyio.h>
#include <stdio.h>

#include "anLCD.h"

bool alphaNumericLCD::mInitialized = false;

alphaNumericLCD::alphaNumericLCD(s_DataPins pins)
{
	if(!mInitialized)
	{
		mInitialized = true;

		mPins = pins;

		powerPorts(mPins);
		setupPins(mPins);


		CYGHWR_HAL_STM32_GPIO_OUT (pins.rsPin, 0);
		CYGHWR_HAL_STM32_GPIO_OUT (pins.rwPin, 0);
		CYGHWR_HAL_STM32_GPIO_OUT (pins.ePin, 1);

		initLCM();

	}
}

cyg_uint32 alphaNumericLCD::getAHB1portBit(cyg_uint32 pinSpec)
{
	cyg_uint32 portNum = ((pinSpec)&0xF0000) >> 16;
	return 1 << portNum;
}

void alphaNumericLCD::powerPorts(s_DataPins pins)
{
	cyg_uint32 portEnableReg = 0;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_AHB1ENR, portEnableReg);

	portEnableReg |= getAHB1portBit(pins.rsPin);
	portEnableReg |= getAHB1portBit(pins.rwPin);
	portEnableReg |= getAHB1portBit(pins.ePin);

	for(cyg_uint8 k = 0; k < 8; k++)
		portEnableReg |= getAHB1portBit( pins.dbPins[k] );

	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_AHB1ENR, portEnableReg);
}

void alphaNumericLCD::setupPins(s_DataPins pins)
{
	CYGHWR_HAL_STM32_GPIO_SET ( pins.rsPin );
	CYGHWR_HAL_STM32_GPIO_SET ( pins.rwPin );
	CYGHWR_HAL_STM32_GPIO_SET ( pins.ePin );

	for(cyg_uint8 k = 0; k < 8; k++)
		CYGHWR_HAL_STM32_GPIO_SET ( pins.dbPins[k] );
}

void alphaNumericLCD::initLCM()
{
	cyg_thread_delay(2);

	writeIntruction(0x3E);
	cyg_thread_delay(1);

	writeIntruction(0x3E);
	cyg_thread_delay(1);

	writeIntruction(0x3E);
	writeIntruction(0x08);
	writeIntruction(0x01);
	cyg_thread_delay(1);

	writeIntruction(0x06);
	writeIntruction(0x0C);
}

void alphaNumericLCD::clear()
{
	writeIntruction(0x01);
	cyg_thread_delay(1);
}

void alphaNumericLCD::setLine(cyg_uint8 line)
{
	setCursor(line, 0);
}

void alphaNumericLCD::setCursor(cyg_uint8 row, cyg_uint8 col)
{
	if(row == 0xFF || col == 0xFF)
		return;

	if(col > 19)
		col = 19;

	switch(row)
	{
	case 1:
		writeIntruction(0x02 + col);
		cyg_thread_delay(1);
		break;
	case 2:
		writeIntruction(0xC0 + col);
		break;
	case 3:
		writeIntruction(0x94 + col);
		break;
	case 4:
		writeIntruction(0xD4 + col);
		break;

	default:
		break;
	}
}

void alphaNumericLCD::showCursor(cyg_uint8 row, cyg_uint8 col)
{
	setCursor(row, col);
	writeIntruction(0x0D);
}

void alphaNumericLCD::hideCursor()
{
	writeIntruction(0x0C);
}

void alphaNumericLCD::writeIntruction(cyg_uint8 instruction)
{
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.rsPin, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.rwPin, 0);

	HAL_DELAY_US(500);

	if(mPins.setDBcb)
		mPins.setDBcb(instruction);
	else
		setDBpins(instruction);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.ePin, 0);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.ePin, 1);
}

char * alphaNumericLCD::format(const char *f,...)
{
    va_list vl;
    va_start(vl,f);
    vsprintf(mString,f,vl);
    va_end(vl);

    return mString;
}

alphaNumericLCD& alphaNumericLCD::operator <<(const char *str)
{
    cyg_uint32 len = strlen(str);

    for(cyg_uint8 k = 0; k < len; k++)
    	putc(str[k]);

    return (*this);
}

void alphaNumericLCD::println(cyg_uint8 line, const char *f,...)
{
	setLine(line);
	va_list vl;
	va_start(vl,f);
	vsprintf(mString,f,vl);
	va_end(vl);

	*this << mString;
}

void alphaNumericLCD::print(const char *f,...)
{
	va_list vl;
	va_start(vl,f);
	vsprintf(mString,f,vl);
	va_end(vl);

	*this << mString;
}

void alphaNumericLCD::print(const char c)
{
	putc(c);
}

void alphaNumericLCD::putc(const char c)
{
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.rsPin, 1);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.rwPin, 0);

	HAL_DELAY_US(200);

	if(mPins.setDBcb)
		mPins.setDBcb(c);
	else
		setDBpins(c);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.ePin, 0);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (mPins.ePin, 1);


}

void alphaNumericLCD::setDBpins(cyg_uint8 data)
{
	for(cyg_uint8 k = 0; k < 8; k++)
	{
		if(data & (1 << k))
			CYGHWR_HAL_STM32_GPIO_OUT (mPins.dbPins[k], 1);
		else
			CYGHWR_HAL_STM32_GPIO_OUT (mPins.dbPins[k], 0);
	}
}

alphaNumericLCD::s_DataPins::s_DataPins()
{
	rsPin = 0xFFFFFFFF;
	rwPin = 0xFFFFFFFF;
	ePin = 0xFFFFFFFF;
	memset(dbPins, 0xFFFFFFFF, sizeof(cyg_uint32) * 8);
	setDBcb = 0;
}

alphaNumericLCD::s_DataPins::s_DataPins(cyg_uint32 rs,
				cyg_uint32 rw,
				cyg_uint32 e,
				cyg_uint32 dbPin0,
				cyg_uint32 dbPin1,
				cyg_uint32 dbPin2,
				cyg_uint32 dbPin3,
				cyg_uint32 dbPin4,
				cyg_uint32 dbPin5,
				cyg_uint32 dbPin6,
				cyg_uint32 dbPin7,
				void (*cb)(cyg_uint8 data))
{
	rsPin = rs;
	rwPin = rw;
	ePin = e;
	dbPins[0] = dbPin0;
	dbPins[1] = dbPin1;
	dbPins[2] = dbPin2;
	dbPins[3] = dbPin3;
	dbPins[4] = dbPin4;
	dbPins[5] = dbPin5;
	dbPins[6] = dbPin6;
	dbPins[7] = dbPin7;

	setDBcb = cb;
}

alphaNumericLCD::~alphaNumericLCD() {
}

