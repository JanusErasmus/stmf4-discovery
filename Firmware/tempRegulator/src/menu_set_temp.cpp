#include <cyg/kernel/diag.h>

#include "F4_RTC.h"
#include "menu_set_temp.h"

cMenuSetTemp::cMenuSetTemp(cLineDisplay * lcd, cLCDmenu * parent) : cLCDmenu(lcd, "Set Temperature", parent)
{
	mTemperature = F4RTC::getBKP(0);
	mCursurPos = 2;
}

void cMenuSetTemp::open()
{
	mDisplay->clear();
	mDisplay->println(1, mHeading);
	mDisplay->println(2,"   %02d C", mTemperature);

	mDisplay->println(4,"*: BACK      #:ENTER");

	mDisplay->showCursor(mCursurPos,3);
}

void cMenuSetTemp::handleButtonPress(char button)
{
	diag_printf("Temp press %c\n", button);

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
			ones = mTemperature % 10;
			mTemperature = ones + (num * 10);
			break;
		case 1:
			mTemperature = mTemperature - ones + num;
			mDisplay->setCursor(mCursurPos,3);
			idx = 0;
			break;
		default:
			break;
		}
		diag_printf("%02d\n", mTemperature);

		return;
	}

	if(button == '#')
	{
		F4RTC::setBKP(0, mTemperature);
		mDisplay->hideCursor();
		mParent->returnParentMenu();
		return;
	}
}

cMenuSetTemp::~cMenuSetTemp()
{
}

