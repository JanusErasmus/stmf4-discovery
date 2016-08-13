#ifndef SRC_NVM_H_
#define SRC_NVM_H_
#include <cyg/kernel/kapi.h>

class cNVM
{
	enum eNVMindex
	{
		NVM_TEMPERATURE = 0,
		NVM_HUMID = 1,
		NVM_INTERVAL = 2
	};

	static cyg_uint32 mTemp;
	static cyg_uint32 mHumid;
	static cyg_uint32 mInterval;

public:
	static void init();

	static cyg_uint32 getTemp(){ return mTemp; };
	static void setTemp(cyg_uint32);

	static cyg_uint32 getHumid(){ return mHumid; };
	static void setHumid(cyg_uint32);

	static cyg_uint32 getInterval(){ return mInterval; };
	static void setInterval(cyg_uint32);
};

#endif /* SRC_NVM_H_ */
