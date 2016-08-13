#ifndef PWM_H_
#define PWM_H_

#include "term.h"

class cPWM
{
public:
	enum ePosition
	{
		PWM_LEFT = 0x1A00,
		PWM_RIGHT = 0x10F0,
		PWM_MIDDLE = 0x1500
	};

private:
	static cPWM* __instance;
	cPWM();

	cyg_uint32 mValue;
	cyg_uint16 mARR;
	cyg_uint32 mEnablePinSPec;

	void setupPort();

public:
	static void init();
	static cPWM* get();

	void setFrequency(cyg_uint32 f);

	float getMax(){return 100; };
	float getMin(){return 0; };

	void setValue(cyg_uint32 duty);
	static void debug(cTerm & t,int argc,char *argv[]);
};

#endif /* RELAY_H_ */
