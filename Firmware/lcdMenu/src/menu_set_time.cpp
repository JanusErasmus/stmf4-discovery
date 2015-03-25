#include <cyg/kernel/diag.h>

#include "menu_set_time.h"
#include "menu_get_number.h"
#include "F4_RTC.h"

cSetTimeMenu::cSetTimeMenu(cLineDisplay * lcd, cLCDmenu* parent) : cLCDmenu(lcd, "SET TIME", parent)
{
	mCursurPos = 3;
	mHours = 0;
	mMinutes = 0;
}

void cSetTimeMenu::open()
{
	time_t now = time(0);
	struct tm*  info = localtime(&now);
	mHours = info->tm_hour;
	mMinutes = info->tm_min;

	diag_printf("%d:%d", mHours, mMinutes);

	mDisplay->clear();
	mDisplay->println(1,mHeading);
	mDisplay->println(2,"   %02d:%02d", mHours, mMinutes);
	mDisplay->println(3,"- Set HOURS");
	mDisplay->println(4,"- Set MINUTES");

	mDisplay->showCursor(mCursurPos,0);
}

void cSetTimeMenu::handleEnter()
{
	switch(mCursurPos)
	{
	case 3:
		mSubMenu = new cGetNumberMenu("HOUR", mHours, mDisplay, this);
		mDisplay->hideCursor();
		mSubMenu->open();
		break;
	case 4:
		mSubMenu = new cGetNumberMenu("MINUTES", mMinutes, mDisplay, this);
		mDisplay->hideCursor();
		mSubMenu->open();
		break;
	default:
		break;
	}

}

void cSetTimeMenu::handleCancel()
{
	if(mParent)
		mParent->returnParentMenu();
}

void cSetTimeMenu::handleUp()
{
	if(--mCursurPos == 2)
			mCursurPos = 3;

		mDisplay->setCursor(mCursurPos,0);
}

void cSetTimeMenu::handleDown()
{
	if(++mCursurPos > 4)
			mCursurPos = 3;

		mDisplay->setCursor(mCursurPos,0);
}

void cSetTimeMenu::returnParentMenu()
{
	if(mSubMenu)
	{
		cyg_uint8 number = ((cGetNumberMenu*)mSubMenu)->getNumber();

		if(number != 0xFF)
		{
			switch(mCursurPos)
			{
			case 3:
				diag_printf("Set HOURS %d\n", number);
				setHours(number);
				break;
			case 4:
				diag_printf("Set Minutes %d\n", number);
				setMinutes(number);
				break;
			default:
				break;
			}
		}

		delete mSubMenu;
		mSubMenu = 0;
	}

	open();
}

void cSetTimeMenu::setHours(cyg_uint8 hours)
{
	time_t now = time(0);
	struct tm*  info = localtime(&now);

	info->tm_hour = hours;
	info->tm_sec = 0;

	time_t then = 0;
	then = mktime(info);
	F4RTC::setTime(&then);
}

void cSetTimeMenu::setMinutes(cyg_uint8 min)
{
	time_t now = time(0);
	struct tm*  info = localtime(&now);

	info->tm_min = min;
	info->tm_sec = 0;

	time_t then = 0;
	then = mktime(info);
	F4RTC::setTime(&then);
}

cSetTimeMenu::~cSetTimeMenu()
{
}

