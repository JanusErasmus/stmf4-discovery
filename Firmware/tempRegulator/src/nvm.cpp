#include <cyg/kernel/kapi.h>

#include "nvm.h"
#include "F4_RTC.h"

cyg_uint32 cNVM::mTemp = 0;
cyg_uint32 cNVM::mHumid = 0;
cyg_uint32 cNVM::mInterval = 0;

void cNVM::init()
{
	mTemp = F4RTC::getBKP(NVM_TEMPERATURE);
	mHumid = F4RTC::getBKP(NVM_HUMID);
	mInterval = F4RTC::getBKP(NVM_INTERVAL);
}

void cNVM::setTemp(cyg_uint32 temp)
{
	mTemp = temp;
	F4RTC::setBKP(NVM_TEMPERATURE, temp);
}

void cNVM::setHumid(cyg_uint32 humid)
{
	mHumid = humid;
	F4RTC::setBKP(NVM_HUMID, humid);
}

void cNVM::setInterval(cyg_uint32 interval)
{
	mInterval = interval;
	F4RTC::setBKP(NVM_INTERVAL, interval);
}
