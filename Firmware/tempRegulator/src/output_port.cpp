#include <cyg/kernel/diag.h>
#include <stdio.h>
#include <stdlib.h>

#include "output_port.h"
#include "definitions.h"
#include "utils.h"

cOutputDriver* cOutputDriver::__instance = NULL;

cOutput::cOutput(cyg_uint32 port, cyg_uint8 activeLow) : mPort(port), mActiveLow(activeLow)
{
	if(mActiveLow)
		CYGHWR_HAL_STM32_GPIO_OUT(mPort,1);

	CYGHWR_HAL_STM32_GPIO_SET (mPort);
}

cyg_bool cOutput::get()
{
	int val;
	CYGHWR_HAL_STM32_GPIO_IN(mPort, &val);

	if(mActiveLow)
		val = !val;

	return val;
}

void cOutput::set()
{
	int val = 1;

	if(mActiveLow)
		val = 0;

	CYGHWR_HAL_STM32_GPIO_OUT(mPort, val);
}

void cOutput::reset()
{
	int val = 0;

	if(mActiveLow)
		val = 1;

	CYGHWR_HAL_STM32_GPIO_OUT(mPort, val);
}

/**
 * Initialize output ports
 *
 * @param portNumbers	A list of the output port numbers PA0 = 0, PA1 = 1 through to PF7 = 87
 * 						Use defined MACROS of input_port.h to generate port numbers INPUTx_(y) where x = A-F and y = 0-7
 * @param portCount		Number of ports in the list
 */
void cOutputDriver::init(cOutput **ports)
{
	diag_printf("Initializing cOutput\n");
	if(!__instance)
	{
		__instance = new cOutputDriver(ports);
	}
}

cOutputDriver* cOutputDriver::get()
{
	return __instance;
}


cOutputDriver::cOutputDriver(cOutput **ports)
{
	mOutputCnt = 0;
	//count how much ports was given
	while(ports[mOutputCnt])
	{
		mOutputCnt++;
	}

	diag_printf("There is %d ports\n", mOutputCnt);

	mOutputList = new cOutput*[mOutputCnt];
	memcpy(mOutputList, ports, sizeof(cOutput*) * mOutputCnt);

}

void  cOutputDriver::showOutputs(cTerm & t,int argc,char *argv[])
{
	if(!__instance)
		return;

	t<<YELLOW("OUTPUT status:\n");
	for(cyg_uint8 k = 0; k < __instance->mOutputCnt; k++)
	{
		if( __instance->mOutputList[k]->get())
			t<<t.format(GREEN("\tO%d: 1\n"), k);
		else
			t<<t.format(RED("\tO%d: 0\n"), k);
	}
}

void cOutputDriver::setOutput(cTerm & t,int argc,char *argv[])
{
	if(!__instance)
		return;

	if(argc  > 2)
	{
		cyg_uint8 port = strtoul(argv[1],NULL,16);
		bool stat = strtoul(argv[2],NULL,16);

		if(port > __instance->mOutputCnt)
		{
			t << RED("invalid PORT");
			return;
		}

		if(stat)
		{
			t<<t.format(YELLOW("SET relay %d\n"), port);
			__instance->mOutputList[port]->set();
		}
		else
		{
			t<<t.format(CYAN("RESET relay %d\n"), port);
			__instance->mOutputList[port]->reset();
		}


	}
	else
	{
		showOutputs(t, argc, argv);
	}
}
