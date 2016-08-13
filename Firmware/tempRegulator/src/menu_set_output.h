#ifndef MENU_SET_OUT_H_
#define MENU_SET_OUT_H_

#include "menu_lcd.h"

class cMenuSetOutput: public cLCDmenu
{
	cyg_uint8 mCursurPos;
	cyg_uint32 mInterval;
	void handleButtonPress(char button);

public:
	cMenuSetOutput(cLineDisplay * lcd, cLCDmenu * parent);
	virtual ~cMenuSetOutput();

	void open();
};

#endif /* MENU_SET_TEMP_H_ */
