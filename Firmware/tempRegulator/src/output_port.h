#ifndef RELAY_H_
#define RELAY_H_

#include "term.h"

class cOutput
{
	cyg_uint32 mPort;
	cyg_bool mActiveLow;

public:
	cOutput(cyg_uint32 port, cyg_uint8 activeLow = 0);


	void setActiveLow(bool activeLow){ mActiveLow = activeLow; };

	cyg_bool get();
	void set();
	void reset();
};

class cOutputDriver
{
	static cOutputDriver* __instance;
	cOutputDriver(cOutput **ports);

	cyg_uint8 mOutputCnt;
	cOutput **mOutputList;

	void setupPorts(cyg_uint32* ports, cyg_uint8 count);

public:
	static void init(cOutput **ports);
	static cOutputDriver* get();

	static void showOutputs(cTerm & t,int argc,char *argv[]);
	static void setOutput(cTerm & t,int argc,char *argv[]);
};

#endif /* RELAY_H_ */
