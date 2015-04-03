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
		CYGHWR_HAL_STM32_GPIO_SET(mColList[k]);
	}
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_SET(mRowList[k]);
	}
	for (int k = 0; k < mRowCnt; k++)
	{
		CYGHWR_HAL_STM32_GPIO_OUT(mRowList[k], 0);
	}
}

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

	diag_printf("Key: %c\n", keys[row][col]);
}

void cKeypad::inputChanged(cyg_bool state)
{
	if(state)
		return;

	int column = readColumn();

	maskColumnInterrupts();

	int rowPins = scanRow(column);

	unMaskColumnInterrupts();

	handleKeyPress(column, rowPins);

}

cKeypad::~cKeypad() {
	// TODO Auto-generated destructor stub
}

