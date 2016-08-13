#include <cyg/kernel/diag.h>
#include <time.h>
#include <stdio.h>

#include "definitions.h"
#include "init.h"
#include "menu_main.h"
#include "menu_set_time.h"
#include "menu_set_output.h"
#include "menu_set_temp.h"

cMainMenu::cMainMenu(cLineDisplay * lcd, cLCDmenu * parent) :
	cLCDmenu(lcd, "Main Menu", parent)
{
	mMenuCnt = 3;
	mCursurPos = 2;
}

void cMainMenu::open()
{
	mDisplay->clear();

	mCursurPos = 3;


	mDisplay->println(2,"       STANDBY");

}

void cMainMenu::updateReading(float temp, float humid, cyg_bool heater, cyg_bool water)
{
	if(!mSubMenu)
	{
		char string[32];

		mDisplay->clear();

		time_t now = time(NULL);
		strftime(string, 16, "     %H:%M", localtime(&now));
		mDisplay->println(1, string);

		sprintf(string, "TEMPERATURE: %0.1f", temp);
		mDisplay->println(2,string);

		sprintf(string, "HUMIDITY   : %0.1f", humid);
		mDisplay->println(3,string);

		sprintf(string, "H: %s    W: %s", heater?"ON ":"OFF", water?"ON ":"OFF");
		mDisplay->println(4,string);
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

