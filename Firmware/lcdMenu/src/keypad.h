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
<<<<<<< HEAD
	int readColumn(int idx = -1);
	void maskColumnInterrupts();
	int scanRow(int row);
=======
	int readColumn();
	void maskColumnInterrupts();
	void setRowInputs();
	void setColumnOutputs();
	int readRow();
>>>>>>> 9a7bf6d4a318dfff7ecc05736f15373019ec73ae
	void unMaskColumnInterrupts();
	void handleKeyPress(int col, int row);

public:
	cKeypad(cyg_uint32* colSpec, cyg_uint8 colCount, cyg_uint32* rowSpec, cyg_uint8 rowCount);
	virtual ~cKeypad();


};

#endif /* SRC_KEYPAD_H_ */
