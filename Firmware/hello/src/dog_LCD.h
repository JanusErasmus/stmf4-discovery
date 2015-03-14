#ifndef SRC_DOG_LCD_H_
#define SRC_DOG_LCD_H_

class dogLCD
{
	static bool mInitialized;

	char mString[32];

	void initLCM();
	void writeIntruction(cyg_uint8 instruction);
	void putc(const char c);

public:
	dogLCD();
	virtual ~dogLCD();

	void clear();
	void setLine(cyg_uint8 line);

	dogLCD& operator<<(const char *);
	char * format(const char *f,...);

};

#endif /* SRC_DOG_LCD_H_ */
