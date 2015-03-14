#include <cyg/kernel/diag.h>
#include <cyg/hal/var_io.h>
#include <cyg/kernel/kapi.h>
#include <ctype.h>
#include <string.h>
#include <cyg/io/ttyio.h>
#include <stdio.h>

#include "dog_LCD.h"

#define RS CYGHWR_HAL_STM32_GPIO(E, 4, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define RW CYGHWR_HAL_STM32_GPIO(E, 5, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define E CYGHWR_HAL_STM32_GPIO(E, 6, GPIO_OUT, 0, PUSHPULL , NONE, 2MHZ)

bool dogLCD::mInitialized = false;

dogLCD::dogLCD()
{
	if(!mInitialized)
	{
		mInitialized = true;

	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(AHB1, GPIOE));
	/*RS*/ CYGHWR_HAL_STM32_GPIO_SET ( RS );
	/*RW*/ CYGHWR_HAL_STM32_GPIO_SET ( RW );
	/*E*/ CYGHWR_HAL_STM32_GPIO_SET ( E );

	/*D0*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E,  8, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D1*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E,  9, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D2*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 10, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D3*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 11, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D4*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 12, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D5*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 13, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D6*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 14, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );
	/*D7*/ CYGHWR_HAL_STM32_GPIO_SET ( CYGHWR_HAL_STM32_GPIO(E, 15, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ) );

	CYGHWR_HAL_STM32_GPIO_OUT (RS, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (RW, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (E, 1);

	initLCM();

	}
}

void dogLCD::initLCM()
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

void dogLCD::clear()
{
	writeIntruction(0x01);
	cyg_thread_delay(1);
}

void dogLCD::setLine(cyg_uint8 line)
{
	switch(line)
	{
	case 1:
		writeIntruction(0x02);
		cyg_thread_delay(1);
		break;
	case 2:
		writeIntruction(0xC0);
		break;

	default:
		break;
	}

}

void dogLCD::writeIntruction(cyg_uint8 instruction)
{
	CYGHWR_HAL_STM32_GPIO_OUT (RS, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (RW, 0);

	HAL_DELAY_US(500);

	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
	reg32 &= (0xFFFF00FF);
	reg32 |= instruction << 8;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (E, 0);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (E, 1);
}

char * dogLCD::format(const char *f,...)
{
    va_list vl;
    va_start(vl,f);
    vsprintf(mString,f,vl);
    va_end(vl);

    return mString;
}

dogLCD& dogLCD::operator <<(const char *str)
{
    cyg_uint32 len = strlen(str);

    for(cyg_uint8 k = 0; k < len; k++)
    	putc(str[k]);

    return(*this);
}

void dogLCD::putc(const char c)
{
	CYGHWR_HAL_STM32_GPIO_OUT (RS, 1);
	CYGHWR_HAL_STM32_GPIO_OUT (RW, 0);

	HAL_DELAY_US(200);

	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
	reg32 &= (0xFFFF00FF);
	reg32 |= c << 8;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (E, 0);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (E, 1);


}

dogLCD::~dogLCD() {
	// TODO Auto-generated destructor stub
}

