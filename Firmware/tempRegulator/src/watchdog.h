#ifndef WD_H_
#define WD_H_
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>

#include "definitions.h"


class cWatchDog
{
    cWatchDog(){};
    virtual ~cWatchDog(){};

public:
    static void start();
    static void kick();
};


#endif /* WATCHDOG_H_ */
