#ifndef ALARM_H_
#define ALARM_H_
#include "term.h"

typedef void (*inputTriggerCB)(cyg_bool state);

class inputListener
{
public:
	inputListener(){};
	virtual ~inputListener(){};
	virtual void inputChanged(cyg_bool state) = 0;
};

class cInput
{
	static cInput* __instance;
	bool mAlarmStart;
	cInput(cyg_uint32* portSpec, cyg_uint8 portCount);

	cyg_uint8 mInputCnt;
	cyg_uint32* mInputList;
	inputListener ** mTriggerCallBacks;

	cyg_interrupt* mPDx_Interrupt;
	cyg_handle_t* mPDx_IntHandle;
	static cyg_uint32 handleISR(cyg_vector_t vector,cyg_addrword_t data);
	static void handleDSR(cyg_vector_t vector,cyg_uint32 count,cyg_addrword_t data);

	void setupPorts(cyg_uint32* ports, cyg_uint8 count);
	void setupInterrupts(cyg_uint32* ports, cyg_uint8 count);
	void enableInterrupt(cyg_uint8 port, cyg_uint8 pin);

public:
	static void init(cyg_uint32* portSpec, cyg_uint8 portCount);
	static cInput* get();

	void start();
	void setListener(cyg_uint8 inputNum, inputListener * trigger);

	cyg_uint8 getPortCount(){ return mInputCnt; };
	bool getPortState(cyg_uint8);

	static void showInputs(cTerm & t,int argc,char *argv[]);
};

#endif /* ALARM_H_ */
