#include <cyg/kernel/diag.h>
#include <time.h>

#include "menu_main.h"
#include "menu_set_time.h"

cMainMenu::cMainMenu(cLineDisplay * lcd, cLCDmenu * parent) : cLCDmenu(lcd, "Main Menu", parent)
{
	mMenuCnt = 3;
	mCursurPos = 2;
}

void cMainMenu::open()
{
	mDisplay->clear();
	mDisplay->println(1,mHeading);

	mCursurPos = 2;
	mSubMenu = 0;

	//list all the sub menus

	char string[16];
	time_t now = time(NULL);

	strftime(string, 16, "%a %d-%m-%Y", localtime(&now));
	mDisplay->println(2, string);

	mDisplay->println(3, "- SETUP PUMP");
	mDisplay->println(4, "- SET TIME");

	mDisplay->showCursor(mCursurPos,0);
}

void cMainMenu::handleEnter()
{
	if(mCursurPos > (mMenuCnt + 1))
		return;

	mDisplay->hideCursor();

	switch(mCursurPos)
	{
	case 3:
		mSubMenu = new cSetTimeMenu(mDisplay, this);
		break;

	default:
		break;
	}

	if(mSubMenu)
		mSubMenu->open();
}

void cMainMenu::handleCancel()
{
	if(mParent)
		mParent->returnParentMenu();
}

void cMainMenu::handleUp()
{
	if(--mCursurPos == 1)
		mCursurPos = mMenuCnt + 1;

	mDisplay->setCursor(mCursurPos,0);

	diag_printf("cursor %d\n", mCursurPos);
}

void cMainMenu::handleDown()
{
	if(++mCursurPos > 4)
		mCursurPos = 2;

	mDisplay->setCursor(mCursurPos,0);

	diag_printf("cursor %d\n", mCursurPos);
}

cMainMenu::~cMainMenu()
{
}

