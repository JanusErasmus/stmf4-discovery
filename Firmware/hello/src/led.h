#ifndef LED_H_
#define LED_H_
#include <cyg/kernel/kapi.h>
#include "definitions.h"

class cLED
{
public :
	struct ledPins_s
	{
		cyg_uint32 greenPinSpec;
		cyg_uint32 redPinSpec;
	};

private:
	enum eLEDcolor
	{
		red,
		green,
		off
	};

	cLED(ledPins_s* pinSpecs, cyg_uint8 pinCount);
	static cLED* __instance;

	cyg_uint8 mLEDCnt;
	ledPins_s* mLEDList;

	cyg_uint8 mLEDStack[LED_STACK_SIZE];
	cyg_thread mLEDThread;
	cyg_handle_t mLEDThreadHandle;
	static void led_thread(cyg_addrword_t args);


	void setupPorts(ledPins_s* ports, cyg_uint8 count);

	void setLED(cyg_uint8, eLEDcolor);
	void animateTest();
	void animateIdle();


public:
	static void init(ledPins_s* pinNumbers, cyg_uint8 pinCount);
	static cLED* get();


	void showIO(cyg_uint8, bool);
};

#endif /* LED_H_ */
