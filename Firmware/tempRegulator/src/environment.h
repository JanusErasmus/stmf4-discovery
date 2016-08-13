#ifndef SRC_ENVIRONMENT_H_
#define SRC_ENVIRONMENT_H_
#include <cyg/kernel/kapi.h>

#include "output_port.h"

class cEnvironment
{
	cOutput *mHeater;
	cOutput *mWater;

	void handleHeater(float temp);
	void handleWater(float humid);

public:
	cEnvironment(cOutput *heater, cOutput *water);
	virtual ~cEnvironment();

	void run(float temp, float humid);

	void getStates(cyg_bool &heater, cyg_bool &water);
};

#endif /* SRC_ENVIRONMENT_H_ */
