#include "uexkull.h"
#include <cuteop.h>

/**
* _UX_diffractionSeries:
*
* This is the bread and butter of Uexkull, the heart and soul!
*/
static void _UX_diffractionSeries(float *vector, uint16_t numElements, float diffractionConstant, bool isSparse)
{
    for (int i = 0; i < numElements; i++)
    {
        if (i != 0)
        {
            if (isSparse)
            {
                vector[i] = vector[i - 1] + (vector[i - 1] * diffractionConstant);
            }
            else {
                vector[i] = vector[i - 1] + (vector[0] * diffractionConstant);
            }
        }
    }
}

void UX_init(uexkull_t *self, float samplerate)
{
    for (int i = 0; i < NUM_BANKS; i++)
    {
        bank_init(&self->bank[i],
            NUM_OSC,
            samplerate,
            0.0f,
            SIN);

        bank_init(&self->lfo[i],
            NUM_OSC,
            samplerate,
            0.0f,
            SIN);

        for (int j = 0; j < NUM_OSC; j++)
        {
            self->freqArray[i][j] = 0.0f;
            self->lfoFreqArray[i][j] = 0.0f;
        }
        self->_diffractionConstant[i] = diffractionConstants[0];
        self->_diffractionWidth[i] = false;
    }
    self->_lfoAmp = 0;
    self->_fundamental = 0;
}

void UX_setWaveform(uexkull_t *self, waveform_t waveform)
{
    for (int i = 0; i < NUM_BANKS; i++)
    {
        bank_setWaveform(&self->bank[i], waveform);
    }
}

void UX_calculateFrequencySeries(uexkull_t *self, float fundamental, uint8_t numConstant, uint8_t numBank, bool isSparse)
{
    self->_fundamental = fundamental;

    self->_diffractionConstant[numBank] = diffractionConstants[numConstant];
    self->freqArray[numBank][0] = fundamental;
    _UX_diffractionSeries(self->freqArray[numBank], NUM_OSC, self->_diffractionConstant[numBank], isSparse);
}

void UX_calculateLFOFrequencies(uexkull_t *self, float lfoFreq, float phaseOffset, float amplitudeOffset)
{
    for (int i = 0;i < NUM_BANKS; i++)
    {
        for (int j = 0; j < NUM_OSC; j++)
        {
            if (lfoFreq < 0.1f)
            {
                self->lfoFreqArray[i][j] = 0 + ((randomOffsets[j] * phaseOffset));
            }
            else {
                self->lfoFreqArray[i][j] = lfoFreq + ((randomOffsets[j] * phaseOffset));
            }
        }
    }

    self->_lfoAmp = amplitudeOffset;
}

float UX_processLeftBank(uexkull_t *self, float fm, float am, float fmAtten, float amAtten, float *gainCurve)
{
    float sig = 0;
    bank_setFrequencies(&(self->bank[0]), self->freqArray[0], fm, fmAtten, NUM_OSC, false);
    bank_setFrequencies(&(self->lfo[0]), self->lfoFreqArray[0], fm, amAtten, NUM_OSC, true);

    float lfoValues[NUM_OSC];
    for (int i = 0; i < NUM_OSC; i++)
    {
        lfoValues[i] = osc_step(&(self->lfo[0].osc[i]), 0) / (float)UINT16_MAX;
    }

    sig += bank_process(&(self->bank[0]), lfoValues, gainCurve);

    return sig;
}

float UX_processRightBank(uexkull_t *self, float fm, float am, float fmAtten, float amAtten, float *gainCurve)
{
    float sig = 0;
    bank_setFrequencies(&(self->bank[1]), self->freqArray[1], fm, fmAtten, NUM_OSC, false);
    bank_setFrequencies(&(self->lfo[1]), self->lfoFreqArray[1], fm, amAtten, NUM_OSC, true);

    float lfoValues[NUM_OSC];
    for (int i = 0; i < NUM_OSC; i++)
    {
        lfoValues[i] = osc_step(&(self->lfo[1].osc[i]), 0) / (float)UINT16_MAX;
    }

    sig += bank_process(&(self->bank[1]), lfoValues, gainCurve);
    return sig;
}
