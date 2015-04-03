#ifndef LCD_MENU_H_
#define LCD_MENU_H_
#include <cyg/kernel/kapi.h>

#include "line_display.h"
#include "keypad.h"

class cLCDmenu : public menuInput
{

	void buttonPress(char button);

protected:
	cLCDmenu* mSubMenu;
	const char* mHeading;
	cLineDisplay * mDisplay;
	cLCDmenu* mParent;

	virtual void handleButtonPress(char button){};

public:
	cLCDmenu(cLineDisplay * lcd, const char* heading, cLCDmenu * parent = 0);

	virtual void open() = 0;

	const char* getHeading(){ return mHeading; };

	virtual void returnParentMenu();

	virtual ~cLCDmenu();
};

#endif /* LCD_MENU_H_ */
