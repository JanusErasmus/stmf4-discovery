#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>

#include "input_port.h"

class menuInput
{
public:
	menuInput(){};
	virtual ~menuInput(){};
	virtual void buttonPress(char button) = 0;
};

class cKeypad : public inputListener
{
	cyg_uint8 mColCnt;
	cyg_uint32* mColList;
	cyg_uint8 mRowCnt;
	cyg_uint32* mRowList;

	menuInput * mMenuInput;

	cyg_mutex_t mEventMutex;
	cyg_cond_t mEventWait;
	cyg_uint8 mRowEvent;
	cyg_uint8 mColumnEvent;

	cyg_uint8 mStack[CYGNUM_HAL_STACK_SIZE_MINIMUM];
	cyg_thread mThread;
	cyg_handle_t mThreadHandle;
	static void thread_func(cyg_addrword_t arg);

	void inputChanged(cyg_bool state);

	void setupPorts();
	int readColumn(int idx = -1);
	void maskColumnInterrupts();
	int scanRow(int row);
	void unMaskColumnInterrupts();
	void handleKeyPress(int col, int row);

public:
	cKeypad(cyg_uint32* colSpec, cyg_uint8 colCount, cyg_uint32* rowSpec, cyg_uint8 rowCount);
	virtual ~cKeypad();

	void setMenu(menuInput * menu){ mMenuInput = menu; };

};

#endif /* SRC_KEYPAD_H_ */
