#include <cyg/kernel/diag.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/var_io.h>
#include <stdio.h>

#include "ht_monitor.h"

cHTmonitor::cHTmonitor(cyg_i2c_device * dev)
{    
    mStatus = false;
    i2c = dev;

    cyg_mutex_init(&mSampleMutex);
}


bool cHTmonitor::getStatus()
{
    return mStatus;
}

/* Samples the Temperature sensor
 * @retval Returns the current sampled value
 */
analogSeq::seqSample cHTmonitor::sampleTemp()
{
    float sample;
    if(getTemp(sample))
        tempSamples.set(sample);
    else
    	diag_printf("sampleTemp failed\n");

    return tempSamples.get();
}

/* Samples the Humid sensor
 * @retval Returns the current sampled value
 */
analogSeq::seqSample cHTmonitor::sampleHumid()
{
    float sample;
    if(getHumid(sample))
        humidSamples.set(sample);
    else
    	diag_printf("sampleHumid failed\n");

    return humidSamples.get();
}

analogSeq::seqSample cHTmonitor::getPortSample(cyg_uint8 port)
{
    analogSeq::seqSample s;

    if(port > 1)
        return s;

    cyg_mutex_lock(&mSampleMutex);

    switch(port)
    {
    case TEMPERATURE:
        s = sampleTemp();
        break;
    case HUMIDITY:
        s = sampleHumid();
        break;
    }

    cyg_mutex_unlock(&mSampleMutex);

    return s;
}

analogSeq::seqSample cHTmonitor::getPortSample(cyg_uint8 port, cyg_uint8 seq)
{
    analogSeq::seqSample s;

    if(port > 1)
        return s;

    switch(port)
    {
    case TEMPERATURE:
        s = tempSamples.get(seq);
        break;
    case HUMIDITY:
        s = humidSamples.get(seq);
        break;
    }

    return s;
}

cyg_uint8 cHTmonitor::read(cyg_uint8 addr, cyg_uint8* buff)
{
    return read(addr, buff, 1);
}

cyg_uint8 cHTmonitor::read(cyg_uint8 addr, cyg_uint8* buff, cyg_uint8 len)
{
    //diag_printf("Reading... 0x%02X\n", addr);

    if(len > 1)
        addr |= 0x80;

    cyg_i2c_transaction_begin(i2c);
    cyg_i2c_transaction_tx(i2c, true, &addr, 1, false);
    len = cyg_i2c_transaction_rx(i2c, true, buff, len, true, true);
    //len =  cyg_i2c_rx(i2c, buff, len);
    cyg_i2c_transaction_end(i2c);

    //cyg_thread_delay(1);

    return len;
}

cyg_uint8 cHTmonitor::write(cyg_uint8 addr, cyg_uint8 data)
{
    cyg_uint8 buff[2];
    buff[0] = addr;
    buff[1] = data;

    return cyg_i2c_tx(i2c, buff, 2);
}

void cHTmonitor::showMeasurements()
{
    analogSeq samples[] = { tempSamples, humidSamples};
    cyg_uint8 cnt = 2;

    for(cyg_uint8 k = 1; k <= cnt; k++)
    {
        printf("    %2d    ", k);
    }
    printf("\n");


    for(int k = 0; k < SEQ_BUFFER_SIZE; k++)
    {
        for(cyg_uint8 i = 0; i < cnt; i++)
            printf("%3d: %4.1f ", samples[i].getBuff()[k].sequence, samples[i].getBuff()[k].value);

        printf("\n");

    }
    printf("\n");
}


cHTmonitor::~cHTmonitor()
{
}

