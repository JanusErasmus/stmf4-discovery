#ifndef HT_STMHTS221_H_
#define HT_STMHTS221_H_

#include "ht_monitor.h"

class cHTstmHTS221 : public cHTmonitor
{
    float mTgain;
    float mToffset;

    float mHgain;
    float mHoffset;

    bool status();

    void readCalibration();

    void readSamples(cyg_int16 &temp, cyg_int16 &humid);
    bool sampleOneShot(cyg_int16 &temp, cyg_int16 &humid);

    bool getTemp(float &temp);
    bool getHumid(float &humid);

public:

    cHTstmHTS221(cyg_i2c_device * dev);
    virtual ~cHTstmHTS221();
};

#endif /* HT_SENSERION_H_ */
