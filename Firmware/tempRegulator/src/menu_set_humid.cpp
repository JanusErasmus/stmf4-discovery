#include <cyg/kernel/diag.h>

#include "nvm.h"
#include "menu_set_humid.h"

cMenuSetHumid::cMenuSetHumid(cLineDisplay * lcd, cLCDmenu * parent) : cLCDmenu(lcd, "Set Humidity", parent)
{
	mHumidity = cNVM::getHumid();
	mCursurPos = 2;
}

void cMenuSetHumid::open()
{
	mDisplay->clear();
	mDisplay->println(1, mHeading);
	mDisplay->println(2,"   %02d %%", mHumidity);

	mDisplay->println(4,"*: BACK      #:ENTER");

	mDisplay->showCursor(mCursurPos,3);
}

void cMenuSetHumid::handleButtonPress(char button)
{
	diag_printf("Humid press %c\n", button);

	if(button == '*')
	{
		mDisplay->hideCursor();
		mParent->returnParentMenu();
		return;
	}

	if(('0' <= button) && (button <= '9'))
	{
		static cyg_uint8 ones = 0;
		static cyg_uint8 idx = 0;
		mDisplay->print(button);

		cyg_uint8 num = button - 0x30;

		switch(idx++)
		{
		case 0:
			ones = mHumidity % 10;
			mHumidity = ones + (num * 10);
			break;
		case 1:
			mHumidity = mHumidity - ones + num;
			mDisplay->setCursor(mCursurPos,3);
			idx = 0;
			break;
		default:
			break;
		}
		diag_printf("%02d\n", mHumidity);

		return;
	}

	if(button == '#')
	{
		cNVM::setHumid(mHumidity);
		mDisplay->hideCursor();
		mParent->returnParentMenu();
		return;
	}
}

cMenuSetHumid::~cMenuSetHumid()
{
}

