#ifndef DEFINITIONS_H
#define	DEFINITIONS_H
#include <cyg/hal/hal_arch.h>

//Stack and thread definitions
#define INIT_PRIORITY		19
#define TERM_PRIORITY		25

//Stack thread sizes
#define INIT_STACK_SIZE		CYGNUM_HAL_STACK_SIZE_TYPICAL
#define TERM_STACK_SIZE		CYGNUM_HAL_STACK_SIZE_TYPICAL


#endif	/* DEFINITIONS_H */

