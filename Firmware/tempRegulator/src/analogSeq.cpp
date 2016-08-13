#include <stdio.h>

#include "analogSeq.h"

const unsigned maxU = 0x7FFFFFFF;
const float __nan = *((float*)&maxU);

analogSeq::analogSeq() : mIndex(0xFF), mSequence(0)
{
}

void analogSeq::set(float val)
{
    if(++mIndex >= SEQ_BUFFER_SIZE)
            mIndex = 0;

    mBuff[mIndex].value = val;
    mBuff[mIndex].sequence = mSequence++;
}

analogSeq::seqSample analogSeq::get()
{
    if(mIndex == 0xFF)
    {
        analogSeq::seqSample none;
        return none;
    }

    return mBuff[mIndex];
}

analogSeq::seqSample analogSeq::get(cyg_uint8 seq)
{
    //search for the sequence number in the list and return it
    int idx = 0;
    while(idx < SEQ_BUFFER_SIZE)
    {
        if(mBuff[idx].sequence == seq)
            return mBuff[idx];

        idx++;
    }

    seqSample sample;
    return sample;
}

analogSeq::seqSample* analogSeq::getBuff()
{
    return mBuff;
}

analogSeq::seqSample::seqSample()
{
    sequence = 0;
    value = NAN;
}

