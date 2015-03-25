#ifndef SRC_ANLCD_H_
#define SRC_ANLCD_H_

#include "line_display.h"

class alphaNumericLCD : public cLineDisplay
{
public:

	/** Data structure of the LCD pins and a optimized DBpin set callback
	 *
	 * If the callback is set, it will be used to set the data bytes of the LCD.
	 * The user could set an optimized function to handle the output data pins.
	 */
	struct s_DataPins
	{
		cyg_uint32 rsPin;
		cyg_uint32 rwPin;
		cyg_uint32 ePin;
		cyg_uint32 dbPins[8];
		void (*setDBcb)(cyg_uint8 data);

		s_DataPins();
		s_DataPins(cyg_uint32 rs,
				cyg_uint32 rw,
				cyg_uint32 e,
				cyg_uint32 dbPin0,
				cyg_uint32 dbPin1,
				cyg_uint32 dbPin2,
				cyg_uint32 dbPin3,
				cyg_uint32 dbPin4,
				cyg_uint32 dbPin5,
				cyg_uint32 dbPin6,
				cyg_uint32 dbPin7,
				void (*cb)(cyg_uint8 data));
	};
private:
	static bool mInitialized;

	char mString[32];
	s_DataPins mPins;

	cyg_uint32 getAHB1portBit(cyg_uint32 pinSpec);
	void powerPorts(s_DataPins pins);
	void setupPins(s_DataPins pins);
	void initLCM();
	void writeIntruction(cyg_uint8 instruction);
	void putc(const char c);
	void setDBpins(cyg_uint8 data);

public:
	alphaNumericLCD(s_DataPins pins);
	virtual ~alphaNumericLCD();

	void clear();
	void setLine(cyg_uint8 line);
	void setCursor(cyg_uint8 row, cyg_uint8 col);
	void showCursor(cyg_uint8 row = 0xFF, cyg_uint8 col = 0xFF);
	void hideCursor();

	alphaNumericLCD& operator<<(const char *);
	char * format(const char *f,...);

	void println(cyg_uint8 line, const char *f,...);

};

#endif /* SRC_ANLCD_H_ */
