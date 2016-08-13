#ifndef CHTMONITOR_H_
#define CHTMONITOR_H_
#include <cyg/io/i2c.h>

#include "analogSeq.h"

class cHTmonitor
{
public:

    enum anaPort
    {
        TEMPERATURE = 0,
        HUMIDITY = 1
    };

protected:

    bool mStatus;

    cyg_i2c_device* i2c;

    analogSeq tempSamples;
    analogSeq humidSamples;

    cyg_mutex_t mSampleMutex;

    cyg_uint8 read(cyg_uint8 addr, cyg_uint8* buff, cyg_uint8 len);
    cyg_uint8 read(cyg_uint8 addr, cyg_uint8* buff);
    cyg_uint8 write(cyg_uint8 addr, cyg_uint8 data);

    analogSeq::seqSample sampleTemp();
    analogSeq::seqSample sampleHumid();

    virtual bool getTemp(float &temp) = 0;
    virtual bool getHumid(float &humid) = 0;

public:

    cHTmonitor(cyg_i2c_device * dev);


    bool getStatus();

    void showMeasurements();

    analogSeq::seqSample getPortSample(cyg_uint8 port, cyg_uint8 seq);
    analogSeq::seqSample getPortSample(cyg_uint8 port);

    virtual ~cHTmonitor();


};

#endif /* CHTMONITOR_H_ */
