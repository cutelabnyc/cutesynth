#include "bank.h"

void bank_init(t_bank *self,
    osc_t *oscillators,
    uint16_t numOsc,
    float sampleRate,
    float fund,
    waveform_t waveform)
{

    self->_numOsc = numOsc;

    self->osc = oscillators;

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

void bank_destroy(t_bank *self) { }

void bank_setFrequencies(t_bank *self, float *frequencies, float fm, float fmAtten, uint16_t numFreq, bool isLFO)
{
    if (!isLFO)
    {
        self->_fund = frequencies[0];
    }

    for (int i = 0; i < numFreq; i++)
    {
        // osc_time(&(self->osc[i]), (float)((frequencies[i] + ((fm + 0.5f) * frequencies[i] * fmAtten)) / (self->_sampleRate / 2)));
        osc_time(&(self->osc[i]), (float)(frequencies[i] / (self->_sampleRate / 2)));

    }
}

void bank_setWaveform(t_bank *self, waveform_t waveform)
{
    for (int i = 0; i < self->_numOsc; i++)
    {
        osc_setWaveform(&(self->osc[i]), waveform);
    }
}

float bank_process(t_bank *self, float *lfoValues, float *gainCurve)
{
    float sig = 0;

    for (int i = 0; i < self->_numOsc; i++)
    {
        sig += (osc_step(&(self->osc[i]), 0) * lfoValues[i]) * gainCurve[i];
    }

    sig /= self->_numOsc;

    return sig;
}
