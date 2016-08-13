#include <math.h>
#include "F4_RTC.h"
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
	cyg_uint8 setTemperature = F4RTC::getBKP(0);

	if(isnan(temp))
	{
		mHeater->reset();
		return;
	}

	if((cyg_uint8)temp < (setTemperature - 1))
	{
		mHeater->set();
	}
	else if((cyg_uint8)temp > (setTemperature))
	{
		mHeater->reset();
	}
}

void cEnvironment::handleWater(float humid)
{
	if(isnan(humid))
	{
		mWater->reset();
		return;
	}

	if((cyg_uint8)humid < (60 - 10))
	{
		mWater->set();
	}
	else if((cyg_uint8)humid > (60 + 1))
	{
		mWater->reset();
	}
}

void cEnvironment::getStates(cyg_bool &heater, cyg_bool &water)
{
	heater = mHeater->get();
	water = mWater->get();
}

cEnvironment::~cEnvironment() {
	// TODO Auto-generated destructor stub
}

