#ifndef ANALOGSEQ_H_
#define ANALOGSEQ_H_
#include <cyg/hal/hal_arch.h>
#include <math.h>

#define SEQ_BUFFER_SIZE 32

extern const float __nan;
#define NAN (float)__nan;

class analogSeq
{

    cyg_uint8 mIndex;
    cyg_uint8 mSequence;

public:

    analogSeq();
//    static float nan;

    struct seqSample
    {
        cyg_uint8 sequence;
        float value;

        seqSample();
        seqSample(int seq, float val){ sequence = seq; value = val; };

    }__attribute__((__packed__));

    seqSample mBuff[SEQ_BUFFER_SIZE];

    void set(float val);

    seqSample get();
    seqSample get(cyg_uint8 seq);

    seqSample* getBuff();

};

#endif /* ANALOGSEQ_H_ */
