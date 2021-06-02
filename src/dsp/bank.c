#include "bank.h"

#define TEST_LFO_FREQ 0.01

void bank_init(t_bank *self,
    uint16_t numOsc,
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

    self->waveform = waveform;

    for (int i = 0; i < numOsc; i++)
    {
        osc_init(&self->osc[i], self->waveform);
    }
}

void bank_destroy(t_bank *self)
{
    free(self->osc);
}

void bank_setFrequencies(t_bank *self, float *frequencies, uint16_t numFreq)
{
    self->_fund = frequencies[0];

    for (int i = 0; i < numFreq; i++)
    {
        osc_time(&(self->osc[i]), (float)(frequencies[i] / (self->_sampleRate / 2)));
    }
}

void bank_setWaveform(t_bank *self, waveform_t waveform)
{
    for (int i = 0; i < self->_numOsc; i++)
    {
        osc_setWaveform(&(self->osc[i]), SAW);
    }
}

float bank_process(t_bank *self, float *gainValues)
{
    float sig = 0;

    for (int i = 0; i < self->_numOsc; i++)
    {
        sig += (osc_step(&(self->osc[i]), 0) * gainValues[i]);
    }

    sig /= self->_numOsc;

    return sig;
}
