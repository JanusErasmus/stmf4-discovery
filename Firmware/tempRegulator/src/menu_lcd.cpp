#include <cyg/kernel/diag.h>

#include "menu_lcd.h"

cLCDmenu::cLCDmenu(cLineDisplay * lcd,  const char* heading, cLCDmenu * parent) :
	mSubMenu(0),
	mHeading(heading),
	mDisplay(lcd),
	mParent(parent)
{
}

void cLCDmenu::buttonPress(char button)
{
	if(mSubMenu)
	{
		mSubMenu->handleButtonPress(button);
		return;
	}

	handleButtonPress(button);
}


void cLCDmenu::returnParentMenu()
{
	if(mSubMenu)
		delete mSubMenu;

	mSubMenu = 0;
	open();
}

cLCDmenu::~cLCDmenu()
{
	if(mSubMenu)
		delete mSubMenu;

	diag_printf("delete super\n");
}

