#include <cyg/kernel/diag.h>
#include <string.h>

#include "keypad.h"

char keys[][4] =
{
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'},
};

cKeypad::cKeypad(cyg_uint32* colSpec, cyg_uint8 colCount, cyg_uint32* rowSpec, cyg_uint8 rowCount)
{
	mColCnt = colCount;
	mColList = new cyg_uint32[mColCnt];
	memcpy(mColList, colSpec, sizeof(cyg_uint32) * mColCnt);

	mRowCnt = rowCount;
	mRowList = new cyg_uint32[mRowCnt];
	memcpy(mRowList, rowSpec, sizeof(cyg_uint32) * mRowCnt);

	setupPorts();

}

void cKeypad::setupPorts()
{
	for (int k = 0; k < mColCnt; k++)
	{
<<<<<<< HEAD
		CYGHWR_HAL_STM32_GPIO_SET(mColList[k]);
	}
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_SET(mRowList[k]);
=======
		CYGHWR_HAL_STM32_GPIO_SET (mColList[k]);
	}
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_SET (mRowList[k]);
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae
	}
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[k], 0);
	}
}

<<<<<<< HEAD
int cKeypad::readColumn(int idx)
{
	if(idx >= 0)
	{
		cyg_uint32 pin = CYGHWR_HAL_STM32_GPIO_BIT(mColList[idx]);
		cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(mColList[idx]);
		cyg_uint32 reg32;
		HAL_READ_UINT32( port + CYGHWR_HAL_STM32_GPIO_IDR, reg32 );

		if(!(reg32 & (1 << pin)))
			return idx;

		return -1;
	}

=======
int cKeypad::readColumn()
{
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae
	for (int k = 0; k < mColCnt; k++)
	{
		cyg_uint32 pin = CYGHWR_HAL_STM32_GPIO_BIT(mColList[k]);
		cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(mColList[k]);
		cyg_uint32 reg32;
		HAL_READ_UINT32( port + CYGHWR_HAL_STM32_GPIO_IDR, reg32 );

		if(!(reg32 & (1 << pin)))
			return k;
	}

	return -1;
}

void cKeypad::maskColumnInterrupts()
{
	cyg_uint32 pin, imrReg;

	HAL_READ_UINT32( CYGHWR_HAL_STM32_EXTI + CYGHWR_HAL_STM32_EXTI_IMR, imrReg);
	for (int k = 0; k < mColCnt; k++)
	{
		pin = CYGHWR_HAL_STM32_GPIO_BIT(mColList[k]);
		imrReg &= ~(1 << pin);
	}
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_EXTI + CYGHWR_HAL_STM32_EXTI_IMR, imrReg);
}

<<<<<<< HEAD
int cKeypad::scanRow(int col)
{
	int activeRow = -1;

	//set all rows except the first and scan through them
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[k], 1);
	}

	//scan columns for the row
	for (int r = 0; r < mRowCnt; r++)
	{

		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[r], 0);

		int column = readColumn(col);

		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[r], 1);

		if(column >= 0)
		{
			activeRow = r;
			break;
		}

	}


	//reset all row pins to low
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[k], 0);
	}

	return activeRow;
=======
void cKeypad::setRowInputs()
{
	cyg_uint32 port, pin;

	for (int k = 0; k < mRowCnt; k++)
	{
		port = ((mRowList[k])&0xF0000) >> 16;
		pin = CYGHWR_HAL_STM32_GPIO_BIT(mRowList[k]);

		cyg_uint32 pinSpec = 0;
		switch(port)
		{
		case 0:
			pinSpec = CYGHWR_HAL_STM32_PIN_IN(A, pin, PULLUP);
			break;
		case 1:
			pinSpec = CYGHWR_HAL_STM32_PIN_IN(B, pin, PULLUP);
			break;
		case 2:
			pinSpec = CYGHWR_HAL_STM32_PIN_IN(C, pin, PULLUP);
			break;
		case 3:
			pinSpec = CYGHWR_HAL_STM32_PIN_IN(D, pin, PULLUP);
			break;
		case 4:
			pinSpec = CYGHWR_HAL_STM32_PIN_IN(E, pin, PULLUP);
			break;

		default:
			break;
		}

		if(pinSpec)
		{
			CYGHWR_HAL_STM32_GPIO_SET(pinSpec);
		}
	}
}

void cKeypad::setColumnOutputs()
{
	cyg_uint32 port, pin;

	for (int k = 0; k < mColCnt; k++)
	{
		port = ((mColList[k])&0xF0000) >> 16;
		pin = CYGHWR_HAL_STM32_GPIO_BIT(mColList[k]);

		cyg_uint32 pinSpec = 0;
		switch(port)
		{
		case 0:
			pinSpec = CYGHWR_HAL_STM32_PIN_OUT(A,  pin, PUSHPULL, NONE, 2MHZ);
			break;
		case 1:
			pinSpec = CYGHWR_HAL_STM32_PIN_OUT(B,  pin, PUSHPULL, NONE, 2MHZ);
			break;
		case 2:
			pinSpec = CYGHWR_HAL_STM32_PIN_OUT(C,  pin, PUSHPULL, NONE, 2MHZ);
			break;
		case 3:
			pinSpec = CYGHWR_HAL_STM32_PIN_OUT(D,  pin, PUSHPULL, NONE, 2MHZ);
			break;
		case 4:
			pinSpec = CYGHWR_HAL_STM32_PIN_OUT(E,  pin, PUSHPULL, NONE, 2MHZ);
			break;

		default:
			break;
		}

		if(pinSpec)
		{
			CYGHWR_HAL_STM32_GPIO_SET(pinSpec);
			CYGHWR_HAL_STM32_GPIO_OUT(pinSpec, 0);
		}
	}
}

int cKeypad::readRow()
{
	for (int k = 0; k < mRowCnt; k++)
	{
		cyg_uint32 pin = CYGHWR_HAL_STM32_GPIO_BIT(mRowList[k]);
		cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(mRowList[k]);
		cyg_uint32 reg32;
		HAL_READ_UINT32( port + CYGHWR_HAL_STM32_GPIO_IDR, reg32 );

		if(!(reg32 & (1 << pin)))
			return k;
	}

	return -1;
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae
}

void cKeypad::unMaskColumnInterrupts()
{
	cyg_uint32 pin, imrReg;

	HAL_READ_UINT32( CYGHWR_HAL_STM32_EXTI + CYGHWR_HAL_STM32_EXTI_IMR, imrReg);
	for (int k = 0; k < mColCnt; k++)
	{
		pin = CYGHWR_HAL_STM32_GPIO_BIT(mColList[k]);
		imrReg |= (1 << pin);
	}
	HAL_WRITE_UINT32( CYGHWR_HAL_STM32_EXTI + CYGHWR_HAL_STM32_EXTI_IMR, imrReg);
}

void cKeypad::handleKeyPress(int col, int row)
{
	if((col < 0) || (row < 0))
		return;

<<<<<<< HEAD
	diag_printf("Key: %c\n", keys[row][col]);
=======
	diag_printf("Key: %c\n", keys[col][row]);
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae
}

void cKeypad::inputChanged(cyg_bool state)
{
	if(state)
		return;

<<<<<<< HEAD
	int column = readColumn();

	maskColumnInterrupts();

	int rowPins = scanRow(column);

	unMaskColumnInterrupts();

	handleKeyPress(column, rowPins);
=======
	int columnPins = readColumn();

	maskColumnInterrupts();
	setRowInputs();
	setColumnOutputs();

	//HAL_DELAY_US(20000);
	cyg_thread_delay(1);

	int rowPins = readRow();

	//restore pin states
	setupPorts();

	unMaskColumnInterrupts();

	handleKeyPress(columnPins, rowPins);
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae

}

cKeypad::~cKeypad() {
	// TODO Auto-generated destructor stub
}

