#ifndef SRC_TEMP_PROBE_H_
#define SRC_TEMP_PROBE_H_

class tempProbe
{
	cyg_uint32 mBuff[32];
	cyg_uint8 mIdx;
	bool mTempValid;

public:
	tempProbe(cyg_uint32 adc_pinspec);
	virtual ~tempProbe();

	void sample();
	double getTemp();
};

#endif /* SRC_TEMP_PROBE_H_ */
