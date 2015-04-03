#include <cyg/kernel/diag.h>
#include <time.h>

#include "menu_main.h"
#include "menu_set_time.h"

cMainMenu::cMainMenu(cLineDisplay * lcd, cLCDmenu * parent) :
	cLCDmenu(lcd, "Main Menu", parent)
{
	mMenuCnt = 3;
	mCursurPos = 2;
}

void cMainMenu::open()
{
	mDisplay->clear();
	mDisplay->println(1,mHeading);

	mCursurPos = 3;


	if(mSubMenu)
		delete mSubMenu;
	mSubMenu = 0;

	//list all the sub menus

	char string[16];
	time_t now = time(NULL);

	strftime(string, 16, "%a %d-%m-%Y", localtime(&now));
	mDisplay->println(2, string);

	mDisplay->println(3, "1: SETUP PUMP");
	mDisplay->println(4, "2: SET TIME");

}

void cMainMenu::handleButtonPress(char button)
{
	diag_printf("Main press %c\n", button);
	switch(button)
	{
	case '1':
		break;
	case '2':
		mSubMenu = new cSetTimeMenu(mDisplay, this);
		mSubMenu->open();
		break;

	default:
		break;
	}
}

cMainMenu::~cMainMenu()
{
}

