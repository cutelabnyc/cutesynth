#include "bank.h"

#define TEST_LFO_FREQ 0.01

void BK_init(bank_t *self,
    uint8_t numOsc,
    float sampleRate,
    float fund,
    waveform_t waveform)
{
    
    self->_numOsc = numOsc;

    self->osc = (osc_t *)malloc(sizeof(osc_t) * self->_numOsc);

    self->_sampleRate = sampleRate;
    self->_fund = fund;

    /*modular_t _modular;*/
    self->_1vo = 0;

    self->waveform = SIN;

    for (int i = 0; i < numOsc; i++)
    {
        osc_init(&self->osc[i]);
    }
}

void BK_destroy(bank_t *self)
{
    free(self->osc);
}

void BK_setFrequencies(bank_t *self, float *vector, uint8_t numFreq)
{
    for (int i = 0; i < numFreq; i++)
    {
        osc_time(&(self->osc[i]), (float)(vector[i] / (self->_sampleRate / 2)));
    }
}

float BK_process(bank_t *self)
{
    float sig = 0;

    for (int i = 0; i < self->_numOsc; i++)
    {
        sig += osc_step(&(self->osc[i]), 0) / self->_numOsc;
    }

    return sig;
}
