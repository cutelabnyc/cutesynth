/**
 * bank.h —— (Max Ardito, 10/07/20)
 *
 * A single oscillator bank...
 */

#ifndef BANK_H
#define BANK_H

#include <stdbool.h>
#include "oscillator.h"

 /**
  * waveform_t: Waveforms selection for bank oscillators
  */

  /**
   * t_bank: Module's main data structure
   */
typedef struct bank
{
    osc_t *osc;

    uint16_t _numOsc; // Number of oscillators in the bank

    float _sampleRate;      // Sampling rate
    float _fund;            // Bank frequency fundamental
    /*modular_t _modular;*/ // Interface for FM/PM/AM functions
    float _1vo;             // 1V/Oct input

    waveform_t waveform; // Waveform for all bank oscillators
} t_bank;

/**
 * Initialize the 'bank' struct
 */
void bank_init(t_bank *self,
    osc_t *oscillators,
    uint16_t numOsc,
    float sampleRate,
    float fund,
    waveform_t waveform);

/**
 * Cleans up the 'bank' struct
 *
 * TODO: Add and describe parameters
 */
void bank_destroy(t_bank *self);

/**
 * Sets the frequencies of the bank with an incoming
 * array of floats
 */
void bank_setFrequencies(t_bank *self, float *frequencies, float fm, float fmAtten, uint16_t numFreq, bool isLFO);

/**
 * Set waveform for all oscillators in bank
 */
void bank_setWaveform(t_bank *self, waveform_t waveform);

/**
 * Process a single sample from Uexkull
 */
float bank_process(t_bank *self, float *gainValues, float *gainCurve);

#endif /* BANK_H */
