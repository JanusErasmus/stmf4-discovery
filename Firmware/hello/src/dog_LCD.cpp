#include <cyg/kernel/diag.h>
#include <cyg/hal/var_io.h>
#include <cyg/kernel/kapi.h>
#include <ctype.h>
#include <string.h>
#include <cyg/io/ttyio.h>
#include <stdio.h>

#include "dog_LCD.h"

#define LCD_RS	CYGHWR_HAL_STM32_GPIO(B, 10, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_RW	CYGHWR_HAL_STM32_GPIO(B, 14, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_E	CYGHWR_HAL_STM32_GPIO(B, 12, GPIO_OUT, 0, PUSHPULL , NONE, 2MHZ)
#define LCD_DB0	CYGHWR_HAL_STM32_GPIO(E,  8, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB1	CYGHWR_HAL_STM32_GPIO(E,  9, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB2	CYGHWR_HAL_STM32_GPIO(E, 10, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB3	CYGHWR_HAL_STM32_GPIO(E, 11, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB4 CYGHWR_HAL_STM32_GPIO(E, 12, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB5 CYGHWR_HAL_STM32_GPIO(E, 13, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB6 CYGHWR_HAL_STM32_GPIO(E, 14, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)
#define LCD_DB7 CYGHWR_HAL_STM32_GPIO(E, 15, GPIO_OUT, 0, OPENDRAIN , NONE, 2MHZ)

bool dogLCD::mInitialized = false;

dogLCD::dogLCD()
{
	if(!mInitialized)
	{
		mInitialized = true;

	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(AHB1, GPIOE));
	CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(AHB1, GPIOB));
	/*RS*/	CYGHWR_HAL_STM32_GPIO_SET ( LCD_RS );
	/*RW*/	CYGHWR_HAL_STM32_GPIO_SET ( LCD_RW );
	/*E*/	CYGHWR_HAL_STM32_GPIO_SET ( LCD_E );

	/*DB0*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB0 );
	/*DB1*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB1 );
	/*DB2*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB2 );
	/*DB3*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB3 );
	/*DB4*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB4 );
	/*DB5*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB5 );
	/*DB6*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB6 );
	/*DB7*/ CYGHWR_HAL_STM32_GPIO_SET ( LCD_DB7 );

	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RS, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RW, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_E, 1);

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
	case 3:
		writeIntruction(0x94);
		break;
	case 4:
		writeIntruction(0xD4);
		break;

	default:
		break;
	}

}

void dogLCD::setDBpins(cyg_uint8 data)
{
	cyg_uint32 reg32;
	HAL_READ_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
	reg32 &= (0xFFFF00FF);
	reg32 |= data << 8;
	HAL_WRITE_UINT32(CYGHWR_HAL_STM32_GPIOE + CYGHWR_HAL_STM32_GPIO_ODR , reg32);
}

void dogLCD::writeIntruction(cyg_uint8 instruction)
{
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RS, 0);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RW, 0);

	HAL_DELAY_US(500);

	setDBpins(instruction);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_E, 0);

	HAL_DELAY_US(500);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_E, 1);
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
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RS, 1);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_RW, 0);

	HAL_DELAY_US(200);

	setDBpins(c);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_E, 0);

	HAL_DELAY_US(200);
	CYGHWR_HAL_STM32_GPIO_OUT (LCD_E, 1);


}

dogLCD::~dogLCD() {
	// TODO Auto-generated destructor stub
}

