#include <cyg/kernel/diag.h>

#include "definitions.h"
#include "menu_set_output.h"
#include "pwm_port.h"
#include "nvm.h"

cMenuSetOutput::cMenuSetOutput(cLineDisplay * lcd, cLCDmenu * parent) : cLCDmenu(lcd, "Set Turn Time", parent)
{
	mInterval = cNVM::getInterval() / 3600;
	mCursurPos = 2;
}

void cMenuSetOutput::open()
{
	mDisplay->clear();
	mDisplay->println(1, mHeading);

	mDisplay->println(2,"   %02d H", mInterval);
	mDisplay->println(4,"*: BACK      #:ENTER");

	mDisplay->showCursor(mCursurPos,3);
}

void cMenuSetOutput::handleButtonPress(char button)
{

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
				ones = mInterval % 10;
				mInterval = ones + (num * 10);
				break;
			case 1:
				mInterval = mInterval - ones + num;
				mDisplay->setCursor(mCursurPos,3);
				idx = 0;
				break;
			default:
				break;
			}
			diag_printf("%02d\n", mInterval);

			return;
		}

		if(button == '#')
		{
			cNVM::setInterval(mInterval * 3600);
			mDisplay->hideCursor();
			mParent->returnParentMenu();
			return;
		}
}

cMenuSetOutput::~cMenuSetOutput()
{
}

