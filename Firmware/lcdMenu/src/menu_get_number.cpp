#include "menu_get_number.h"

cGetNumberMenu::cGetNumberMenu(const char* numberName, cyg_uint8 number, cLineDisplay * lcd, cLCDmenu* parent) :
	cLCDmenu(lcd, numberName, parent),
	mNumber(number)
{
}

void cGetNumberMenu::open()
{
	mDisplay->clear();
	mDisplay->println(1,"Set: %s", mHeading);

	mDisplay->println(3," [%03d]", mNumber);

}

void cGetNumberMenu::handleEnter()
{
	if(mParent)
			mParent->returnParentMenu();
}

void cGetNumberMenu::handleCancel()
{
	mNumber = 0xFF;
	if(mParent)
			mParent->returnParentMenu();
}

void cGetNumberMenu::handleUp()
{
	if(++mNumber > 250)
		mNumber = 0;

	mDisplay->println(3," [%03d]", mNumber);
}

void cGetNumberMenu::handleDown()
{
	if(--mNumber > 250)
		mNumber = 0;

	mDisplay->println(3," [%03d]", mNumber);
}

cGetNumberMenu::~cGetNumberMenu()
{
}

