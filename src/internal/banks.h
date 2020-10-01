#ifndef BANKS_H
#define BANKS_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        float *freq;
        float mult;

        uint8_t numOsc;
    } t_banks;

    void banks_init(t_banks *self, uint8_t numOsc);

    void banks_setFreq(t_banks *self, float freq);
    void banks_setMult(t_banks *self, float mult);
    //void banks_setNumOsc(t_banks *self, uint8_t numOsc);

#ifdef __cplusplus
}
#endif

#endif // BANKS_H