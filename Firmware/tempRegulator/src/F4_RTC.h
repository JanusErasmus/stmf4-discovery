#ifndef F4_RTC_H_
#define F4_RTC_H_
#include <term.h>

#include <time.h>

class F4RTC
{
	static bool enterInitMode();
	static bool exitInitMode();
	static bool disableBKProtect();
	static void enableBKProtect();

	static bool syncTime(time_t t = 0);

public:
	F4RTC();
	virtual ~F4RTC();

	static bool init();

	static bool hasBeenSet();

	static time_t getTime();
	static bool setTime(cyg_uint8 yy, cyg_uint8 mm, cyg_uint8 dd, cyg_uint8 h, cyg_uint8 m, cyg_uint8 s);
	static bool setTime(time_t* t);

	static cyg_uint32 getBKP(cyg_uint8 reg);
	static void setBKP(cyg_uint8 reg, cyg_uint32 data);

	static void syncEcosTime(cTerm & term, int argc,char * argv[]);

};

#endif /* F4_RTC_H_ */
