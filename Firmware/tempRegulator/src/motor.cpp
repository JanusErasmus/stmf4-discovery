#include <cyg/kernel/diag.h>

#include "motor.h"

cMotor *cMotor::__instance = 0;

void cMotor::init(cOutput *power, cOutput *dir)
{
	if(!__instance)
		__instance = new cMotor(power, dir);
}

cMotor::cMotor(cOutput *power, cOutput *dir) : mPower(power), mDirection(dir)
{
	mPosition = true;
}

void cMotor::rotate()
{
	diag_printf("cMotor: rotating...\n");
	if(mPosition)
	{
		mPosition = false;
		mDirection->set();
	}
	else
	{
		mPosition = true;
		mDirection->reset();
	}
	mPower->set();

	//wait 10s
	cyg_thread_delay(1000);
	mPower->reset();
	mDirection->reset();


	diag_printf("cMotor: rotate done\n");
}

void cMotor::debugMotor(cTerm & t,int argc,char *argv[])
{
	if(!__instance)
		return;

	__instance->rotate();

}

cMotor::~cMotor()
{
}

