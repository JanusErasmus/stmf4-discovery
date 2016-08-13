#include <math.h>
#include "nvm.h"
#include "environment.h"

cEnvironment::cEnvironment(cOutput *heater, cOutput *water) : mHeater(heater), mWater(water)
{

}

void cEnvironment::run(float temp, float humid)
{
	handleHeater(temp);
	handleWater(humid);
}

void cEnvironment::handleHeater(float temp)
{
	cyg_uint8 setTemperature = cNVM::getTemp();

	if(isnan(temp))
	{
		mHeater->reset();
		return;
	}

	if(temp < (float)(setTemperature - 1))
	{
		mHeater->set();
	}
	else if(temp > (float)(setTemperature))
	{
		mHeater->reset();
	}
}

void cEnvironment::handleWater(float humid)
{
	cyg_uint8 setHumid = cNVM::getHumid();

	if(isnan(humid))
	{
		mWater->reset();
		return;
	}

	if(humid < (float)(setHumid - 10))
	{
		mWater->set();
	}
	else if(humid > (float)(setHumid))
	{
		mWater->reset();
	}
}

void cEnvironment::getStates(cyg_bool &heater, cyg_bool &water)
{
	heater = mHeater->get();
	water = mWater->get();
}

cEnvironment::~cEnvironment()
{
}

