#include <cyg/kernel/diag.h>

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
	mHours = info->tm_hour;
	mMinutes = info->tm_min;

	diag_printf("%d:%d", mHours, mMinutes);

	mDisplay->clear();
	mDisplay->println(1,mHeading);
	mDisplay->println(2,"   %02d:%02d", mHours, mMinutes);
	mDisplay->println(3,"- Set HOURS");
	mDisplay->println(4,"- Set MINUTES");

	mDisplay->showCursor(mCursurPos,3);
}

void cSetTimeMenu::handleButtonPress(char button)
{
	diag_printf("Time press %c\n", button);

	if(button == '*')
	{
		mDisplay->hideCursor();
		mParent->open();
		return;
	}

	mDisplay->print(button);
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
	diag_printf("Time delete\n");
}

