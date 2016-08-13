#ifndef Main_MENU_H_
#define Main_MENU_H_
#include "menu_lcd.h"

class cMainMenu : public cLCDmenu
{
	cyg_uint8 mMenuCnt;
	cyg_uint8 mCursurPos;

	float mPrevTemp;
	float mPrevHumid;
	cyg_uint32 mRotation;
	cyg_uint8 mPrevPins;

	void fillRotation(char *string);

	void handleButtonPress(char button);

public:
	cMainMenu(cLineDisplay * lcd, cLCDmenu * parent);

	void open();
	void updateReading(float temp, float humid, cyg_bool heater, cyg_bool water, cyg_uint32 rotation);


	virtual ~cMainMenu();
};

#endif /* Main_MENU_H_ */
