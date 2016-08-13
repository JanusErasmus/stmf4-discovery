#ifndef MENU_SET_TEMP_H_
#define MENU_SET_TEMP_H_

#include "menu_lcd.h"

class cMenuSetTemp: public cLCDmenu
{
	cyg_uint8 mTemperature;
	cyg_uint8 mCursurPos;

	void handleButtonPress(char button);

public:
	cMenuSetTemp(cLineDisplay * lcd, cLCDmenu * parent);
	virtual ~cMenuSetTemp();

	void open();
};

#endif /* MENU_SET_TEMP_H_ */
