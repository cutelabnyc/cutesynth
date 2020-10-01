#include "banks.h"

void banks_init(t_banks *self, uint8_t numOsc)
{
    self->numOsc = numOsc;
    self->mult = 0;

    self->freq = (float *)malloc(sizeof(float) * self->numOsc);
}

void banks_setFreq(t_banks *self, float freq)
{
    //if (self->freq == NULL)
    //{

    //}

    self->freq[0] = freq;
    for (int i = 1; i < self->numOsc; i++)
    {
        self->freq[i] += (self->freq[i - 1] * self->mult);
    }
}

void banks_setMult(t_banks *self, float mult)
{
    self->mult = mult;
    banks_setFreq(self, *self->freq);
}