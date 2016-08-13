#ifndef SRC_MOTOR_H_
#define SRC_MOTOR_H_
#include <cyg/kernel/kapi.h>

#include "term.h"
#include "output_port.h"

class cMotor
{
	static cMotor * __instance;

	cOutput *mPower;
	cOutput *mDirection;

	cyg_bool mPosition;


	cMotor(cOutput *power, cOutput *dir);

public:
	static void init(cOutput *power, cOutput *dir);
	static cMotor *get(){ return __instance; };
	virtual ~cMotor();

	void rotate();

	static void debugMotor(cTerm & t,int argc,char *argv[]);
};

#endif /* SRC_MOTOR_H_ */
