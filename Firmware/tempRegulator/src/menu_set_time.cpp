#include <cyg/kernel/diag.h>
#include <stdlib.h>

#include "menu_set_time.h"

#include "F4_RTC.h"

cSetTimeMenu::cSetTimeMenu(cLineDisplay * lcd, cLCDmenu* parent) :
	cLCDmenu(lcd, "SET TIME", parent)
{
	mCursurPos = 2;
	mHours = 0;
	mMinutes = 0;
}

void cSetTimeMenu::open()
{
	time_t now = time(0);
	struct tm*  info = localtime(&now);

	//diag_printf("%d:%d\n", mHours, mMinutes);

	mDisplay->clear();
	mDisplay->println(1,mHeading);
	mDisplay->println(2,"   %02d:%02d", info->tm_hour, info->tm_min);

	mDisplay->println(4,"*: BACK      #:ENTER");

	mDisplay->showCursor(mCursurPos,3);
}

void cSetTimeMenu::handleButtonPress(char button)
{
		if(button == '*')
	{
		mDisplay->hideCursor();
		mParent->returnParentMenu();
		return;
	}

	if(('0' <= button) && (button <= '9'))
	{
		static cyg_uint8 idx = 0;
		mDisplay->print(button);

		cyg_uint8 num = button - 0x30;

		switch(idx++)
		{
		case 0:
			mHours += num * 10;
		break;
		case 1:
			mHours += num;
			mDisplay->setCursor(mCursurPos,6);
		break;
		case 2:
			mMinutes += num * 10;
			break;
		case 3:
			mMinutes += num;
			mDisplay->setCursor(mCursurPos,3);
			idx = 0;
		break;
		}


		diag_printf("%02d:%02d\n", mHours, mMinutes);

		return;
	}

	if(button == '#')
	{
		setTime();
		mDisplay->hideCursor();
		mParent->returnParentMenu();
		return;
	}
}


void cSetTimeMenu::setTime()
{
	if(!mHours)
		return;

	time_t now = time(0);
	struct tm*  info = localtime(&now);

	info->tm_hour = mHours;
	info->tm_min = mMinutes;
	info->tm_sec = 0;

	time_t then = 0;
	then = mktime(info);
	F4RTC::setTime(&then);
}

cSetTimeMenu::~cSetTimeMenu()
{
}

