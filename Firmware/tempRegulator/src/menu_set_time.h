#ifndef SET_TIME_MENU_H_
#define SET_TIME_MENU_H_

#include "menu_lcd.h"

class cSetTimeMenu : public cLCDmenu
{
	cyg_uint8 mCursurPos;
	cyg_uint8 mHours;
	cyg_uint8 mMinutes;

	void setTime();

	void handleButtonPress(char button);

public:
	cSetTimeMenu(cLineDisplay * lcd, cLCDmenu* parent = 0);

	void open();

	virtual ~cSetTimeMenu();
};

#endif /* SET_TIME_MENU_H_ */
