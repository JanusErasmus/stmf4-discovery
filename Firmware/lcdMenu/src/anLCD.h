#ifndef SRC_ANLCD_H_
#define SRC_ANLCD_H_

class alphaNumericLCD
{
	static bool mInitialized;

	char mString[32];

	void initLCM();
	void setDBpins(cyg_uint8 data);
	void writeIntruction(cyg_uint8 instruction);
	void putc(const char c);

public:
	alphaNumericLCD();
	virtual ~alphaNumericLCD();

	void clear();
	void setLine(cyg_uint8 line);

	alphaNumericLCD& operator<<(const char *);
	char * format(const char *f,...);

};

#endif /* SRC_ANLCD_H_ */
