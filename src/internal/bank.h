/**
 * bank.h —— (Max Ardito, 10/07/20)
 *
 * A single oscillator bank...
 */

#ifndef BANK_H
#define BANK_H

#include <stdlib.h>
#include "oscillator.h"

 /**
  * waveform_t: Waveforms selection for bank oscillators
  */
typedef enum
{
    SIN,
    SQUARE,
    TRI,
    SAW
} waveform_t;

/**
 * bank_t: Module's main data structure
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
} bank_t;

/**
 * Initialize the 'bank' struct
 */
void bank_init(bank_t *self,
    uint16_t numOsc,
    float sampleRate,
    float fund,
    waveform_t waveform);

/**
 * Frees the 'bank' struct
 *
 * TODO: Add and describe parameters
 */
void bank_destroy(bank_t *self);

/**
 * Sets the frequencies of the bank with an incoming
 * array of floats
 */
void bank_setFrequencies(bank_t *self, float *vector, uint16_t numFreq);

/**
 * Process a single sample from Uexkull
 */
float bank_process(bank_t *self);

#endif /* BANK_H */
