#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_
#include <cyg/kernel/kapi.h>

#include "input_port.h"

class cKeypad : public inputListener
{
	cyg_uint8 mColCnt;
	cyg_uint32* mColList;
	cyg_uint8 mRowCnt;
	cyg_uint32* mRowList;

	void inputChanged(cyg_bool state);

	void setupPorts();
	int readColumn();
	void maskColumnInterrupts();
	void setRowInputs();
	void setColumnOutputs();
	int readRow();
	void unMaskColumnInterrupts();
	void handleKeyPress(int col, int row);

public:
	cKeypad(cyg_uint32* colSpec, cyg_uint8 colCount, cyg_uint32* rowSpec, cyg_uint8 rowCount);
	virtual ~cKeypad();


};

#endif /* SRC_KEYPAD_H_ */