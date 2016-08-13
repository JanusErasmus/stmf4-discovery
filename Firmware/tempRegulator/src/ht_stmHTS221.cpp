#include <cyg/kernel/diag.h>
#include <stdio.h>

#include "ht_stmHTS221.h"

//HTS221 commands
#define WHO_AM_I        0x0F
#define AV_CONF            0x10
#define CTRL_REG1        0x20
#define CTRL_REG2        0x21
#define CTRL_REG3        0x22
#define STATUS_REG        0x27
#define HUMIDITY_OUT_L    0x28
#define HUMIDITY_OUT_H    0x29
#define TEMP_OUT_L        0x2A
#define TEMP_OUT_H        0x2B

#define AV_CONF_T(_reg)            _reg >> 3
#define AV_CONF_H(_reg)            _reg & 0x07


#define CTRL_REG1_PD(_reg)        _reg & (1 << 7)
#define CTRL_REG1_BDU(_reg)        _reg & (1 << 2)
#define CTRL_REG1_ODR(_reg)        _reg & 0x03

#define CTRL_REG2_ONESHOT(_reg)    _reg & (1 << 0)

#define STATUS_REG_DATA_AVAILABLE(_reg)    ((_reg & 0x03) == 0x03)

cHTstmHTS221::cHTstmHTS221(cyg_i2c_device * dev) : cHTmonitor(dev)
{
    mTgain = 1;
    mToffset = 0;
    mStatus = status();
}

bool cHTstmHTS221::status()
{
    cyg_uint8 stat;

    if(!read(WHO_AM_I, &stat))
        return false;

    diag_printf("HTS221 says I_AM: 0x%02X\n", stat);
    if(stat != 0xBC)
        return false;

    readCalibration();

    //switch on the sensor
    write(CTRL_REG1, 0x84);

    read(CTRL_REG1, &stat);
    //diag_printf("CTRL_REG1: 0x%02X\n", stat);

    if(CTRL_REG1_PD(stat))
        diag_printf(" - Device ON\n");
    else
        diag_printf(" - Device Powered down\n");

    if(CTRL_REG1_BDU(stat))
        diag_printf(" - Updates when both HIGH and LOW was read\n");
    else
        diag_printf(" - Updated continuously\n");

    switch(CTRL_REG1_ODR(stat))
    {
    case 0x00:
        diag_printf(" - One shot output\n");
        break;
    case 0x01:
        diag_printf(" - 1Hz output rate\n");
            break;
    case 0x02:
        diag_printf(" - 7Hz output rate\n");
            break;
    case 0x03:
        diag_printf(" - 12.35Hz output rate\n");
            break;
    }


    //set Average samples for Humidity and Temperature to 128 samples
    stat = (0x06 << 3) | 0x05;
    write(AV_CONF, stat);

    read(AV_CONF, &stat);

    switch(AV_CONF_T(stat))
    {
    case 0x00:
        diag_printf(" - AVG count T: 2");
        break;
    case 0x01:
        diag_printf(" - AVG count T: 4");
        break;
    case 0x02:
        diag_printf(" - AVG count T: 8");
        break;
    case 0x03:
        diag_printf(" - AVG count T: 16");
        break;
    case 0x04:
        diag_printf(" - AVG count T: 32");
        break;
    case 0x05:
        diag_printf(" - AVG count T: 64");
        break;
    case 0x06:
        diag_printf(" - AVG count T: 128");
        break;
    case 0x07:
        diag_printf(" - AVG count T: 256");
        break;
    }

    switch(AV_CONF_H(stat))
    {
    case 0x00:
        diag_printf(" H: 4\n");
        break;
    case 0x01:
        diag_printf(" H: 8\n");
        break;
    case 0x02:
        diag_printf(" H: 16\n");
        break;
    case 0x03:
        diag_printf(" H: 32\n");
        break;
    case 0x04:
        diag_printf(" H: 64\n");
        break;
    case 0x05:
        diag_printf(" H: 128\n");
        break;
    case 0x06:
        diag_printf(" H: 256\n");
        break;
    case 0x07:
        diag_printf(" H: 512\n");
        break;
    }

    return true;

}

//22 read function
void cHTstmHTS221::readSamples(cyg_int16 &temp, cyg_int16 &humid)
{
        cyg_uint8 byte;
        read(TEMP_OUT_L, &byte);
        temp = byte;
        read(TEMP_OUT_H, &byte);
        temp |= (byte << 8);
        read(HUMIDITY_OUT_L, &byte);
        humid = byte;
        read(HUMIDITY_OUT_H, &byte);
        humid |= (byte << 8);
//        printf("HTS: t%5d h%5d\n", temp, humid);
}

bool cHTstmHTS221::sampleOneShot(cyg_int16 &temp, cyg_int16 &humid)
{
    cyg_bool stat = false;;
    cyg_uint8 status;

    //single measurement
    write(CTRL_REG2, 0x01);

    cyg_int16 cnt = 200;
    do
    {
        cyg_thread_delay(50);
        //wait for sample ready
        read(STATUS_REG, &status);
        //diag_printf("HTS: status 0x%02X\n", status);

    }while(!(STATUS_REG_DATA_AVAILABLE(status)) && (--cnt));

    if(cnt == 0)
    {
        //printf("HTS: T timed out\n");
    }
    else
    {
        stat = true;

        cyg_int16 prevTemp;
        cyg_int16 prevHumid;
//        read(TEMP_OUT_L, (cyg_uint8*)&prevTemp, 2);
//        read(HUMIDITY_OUT_L, (cyg_uint8*)&prevHumid, 2);
        readSamples(prevTemp, prevHumid);

        for(cyg_uint8 k =0 ; k < 3; k++)
        {
//            read(TEMP_OUT_L, (cyg_uint8*)&temp, 2);
//            read(HUMIDITY_OUT_L, (cyg_uint8*)&humid, 2);
            readSamples(temp, humid);

            if((humid != prevHumid) || (temp != prevTemp))
            {
                stat = false;
                break;
            }
        }
    }

    return stat;
}

bool cHTstmHTS221::getTemp(float &temp)
{
    cyg_int16 outHumid, outTemp;
    if(!sampleOneShot(outTemp, outHumid))
        return false;

    //printf("HTS: T %d\n", outTemp);

    temp = ((mTgain * outTemp) + mToffset) / 8;
    //printf("\tMeasured %f C\n", temp);

    if((-20.0 < temp) && (temp < 110.0))
    	return true;

    temp = NAN;
    return false;
}

bool cHTstmHTS221::getHumid(float &humid)
{
	cyg_int16 outHumid, outTemp;
	if(!sampleOneShot(outTemp, outHumid))
		return false;

	//printf("HTS: H %d\n", outHumid);

	humid = ((mHgain * outHumid) + mHoffset) / 2;
	//printf("\tMeasured %f %%\n", humid);

	//((0.0 < humid) && (humid <= 100.0))
		return true;

	humid = NAN;
	return false;
}

void cHTstmHTS221::readCalibration()
{
    cyg_uint8 H0_rH_x2 = 0;
    cyg_uint8 H1_rH_x2 = 0;
    cyg_uint16 T0_degC_x8 = 0;
    cyg_uint16 T1_degC_x8 = 0;
    cyg_int16 H0_T0_OUT = 0;
    cyg_int16 H1_T0_OUT = 0;
    cyg_int16 T0_OUT = 0;
    cyg_int16 T1_OUT = 0;

    read(0x30, &H0_rH_x2);
//    diag_printf("H0_rH: 0x%02X\n", H0_rH_x2);

    read(0x31, &H1_rH_x2);
//    diag_printf("H1_rH: 0x%02X\n", H1_rH_x2);

    read(0x32,(cyg_uint8*)&T0_degC_x8); //diag_printf("T0_deg byteL: 0x%02X\n", T0_degC_x8);
    read(0x33, (cyg_uint8*)&T1_degC_x8); //diag_printf("T1_deg byteL: 0x%02X\n", T1_degC_x8);

    cyg_uint8 byte;
    read(0x35, &byte); //diag_printf("MSB: 0x%02X\n", byte);
    T0_degC_x8 |= ((byte & 0x03) << 8);
//    diag_printf("\tT0_degC: %d\n", T0_degC_x8);

    T1_degC_x8 |= ((byte & 0x0C) << 6);
//    diag_printf("\tT1_degC: %d\n", T1_degC_x8);


    read(0x36, &byte); //diag_printf("H0 byteL: 0x%02X\n", byte);
    H0_T0_OUT = byte;
    read(0x37, &byte); //diag_printf("H0 byteH: 0x%02X\n", byte);
    H0_T0_OUT |=  (byte << 8);
//    diag_printf("\tH0_TO_OUT: %d\n", H0_T0_OUT);

    read(0x3A, &byte); //diag_printf("H1 byteL: 0x%02X\n", byte);
    H1_T0_OUT = byte;
    read(0x3B, &byte); //diag_printf("H1 byteH: 0x%02X\n", byte);
    H1_T0_OUT |=  (byte << 8);
//    diag_printf("\tH1_TO_OUT: %d\n", H1_T0_OUT);

    read(0x3C, &byte); //diag_printf("T0 byteL: 0x%02X\n", byte);
    T0_OUT = byte;
    read(0x3D, &byte); //diag_printf("T0 byteH: 0x%02X\n", byte);
    T0_OUT |=  (byte << 8);
//    diag_printf("\tTO_OUT: %d\n", T0_OUT);

    read(0x3E, &byte); //diag_printf("T1 byteL: 0x%02X\n", byte);
    T1_OUT = byte;
    read(0x3F, &byte); //diag_printf("T1 byteH: 0x%02X\n", byte);
    T1_OUT |=  (byte << 8);
//    diag_printf("\tT1_OUT: %d\n",T1_OUT);

    mTgain = ((float)T1_degC_x8 - (float)T0_degC_x8) / ((float)T1_OUT - (float)T0_OUT);
    //printf("Temp gain: %f\n", mTgain);
    mToffset = (float)T0_degC_x8 - (mTgain * (float)T0_OUT);
    //printf("Temp offset: %f\n", mToffset);

    mHgain = ((float)H1_rH_x2 - (float)H0_rH_x2) / ((float)H1_T0_OUT - (float)H0_T0_OUT);
    //printf("H gain: %f\n", mHgain);
    mHoffset = (float)H0_rH_x2 - (mTgain * (float)H0_T0_OUT);;

}

cHTstmHTS221::~cHTstmHTS221()
{

}

