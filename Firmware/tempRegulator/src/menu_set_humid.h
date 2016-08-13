#ifndef MENU_SET_HUMID_H_
#define MENU_SET_HUMID_H_

#include "menu_lcd.h"

class cMenuSetHumid: public cLCDmenu
{
	cyg_uint8 mHumidity;
	cyg_uint8 mCursurPos;

	void handleButtonPress(char button);

public:
	cMenuSetHumid(cLineDisplay * lcd, cLCDmenu * parent);
	virtual ~cMenuSetHumid();

	void open();
};

#endif /* MENU_SET_HUMID_H_ */
