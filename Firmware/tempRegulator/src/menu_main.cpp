#include <cyg/kernel/diag.h>
#include <time.h>
#include <stdio.h>

#include "definitions.h"
#include "init.h"
#include "nvm.h"
#include "menu_main.h"
#include "menu_set_time.h"
#include "menu_set_output.h"
#include "menu_set_temp.h"
#include "menu_set_humid.h"

cMainMenu::cMainMenu(cLineDisplay * lcd, cLCDmenu * parent) :
	cLCDmenu(lcd, "Main Menu", parent)
{
	mPrevTemp = NAN;
	mPrevHumid = NAN;
	mPrevPins = 0;
	mRotation = 0;
	mCursurPos = 0;
	mMenuCnt = 0;
}

void cMainMenu::fillRotation(char *string)
{
	if(mRotation > 3600)//hours left
	{
		int hours = mRotation/3600;
		cyg_uint8 min = (mRotation - (hours * 3600)) / 60;
		sprintf(string, "%2dh%02dm", hours, min);
	}
	else if(mRotation > 60) //minutes left
	{
		sprintf(string, "%2dm%02ds", (int)mRotation/60, mRotation%60);
	}
	else //seconds left
	{
		sprintf(string, "%5ds", mRotation);
	}
}


void cMainMenu::open()
{
	char timeString[16];
	cyg_uint32 setTemp = cNVM::getTemp();
	cyg_uint32 setHumid = cNVM::getHumid();

	mDisplay->clear();

	time_t now = time(NULL);
	strftime(timeString, 16, "%H:%M", localtime(&now));

	char rotationString[16];
	fillRotation(rotationString);
	mDisplay->println(1, "       %s", timeString);

	cyg_bool heater = (mPrevPins & 0x01);
	cyg_bool water = (mPrevPins & 0x02);
	mDisplay->println(2, "TEMP (%2d): %0.1f  %s", setTemp, mPrevTemp, heater?"ON ":"OFF");
	mDisplay->println(3, "HUMID(%2d): %0.1f  %s", setHumid, mPrevHumid, water?"ON ":"OFF");
	mDisplay->println(4, "NEXT TURN: %s", rotationString);

}

void cMainMenu::updateReading(float temp, float humid, cyg_bool heater, cyg_bool water, cyg_uint32 rotation)
{
	mPrevTemp = temp;
	mPrevHumid = humid;
	mRotation = rotation;

	mPrevPins = 0;
	if(heater)
		mPrevPins |= 0x01;
	if(water)
		mPrevPins |= 0x02;

	if(!mSubMenu)
	{
		open();
	}
}

void cMainMenu::handleButtonPress(char button)
{
	diag_printf("Main press %c\n", button);
	switch(button)
	{
	case 'A':
		mSubMenu = new cMenuSetOutput(mDisplay, this);
		mSubMenu->open();
		break;
	case 'B':
		mSubMenu = new cSetTimeMenu(mDisplay, this);
		mSubMenu->open();
		break;
	case 'C':
		mSubMenu = new cMenuSetTemp(mDisplay, this);
		mSubMenu->open();
		break;
	case '5':
		mSubMenu = new cMenuSetHumid(mDisplay, this);
		mSubMenu->open();
		break;

	case 'D':
		mDisplay->clear();
		mDisplay->println(1,"      ROTATE...");
		mDisplay->println(2,"       STANDBY");
		cInit::forceRotate();
		break;

	default:
		break;
	}
}

cMainMenu::~cMainMenu()
{
}

