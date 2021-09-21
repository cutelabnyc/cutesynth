/**
 * messd.h —— (Max Ardito, 07/09/20)
 *
 * Class representing the entire module's
 * functionality. Instantiated globally in
 * [/src/main.cpp], initialized, and then used
 * for CV processing in the main loop.
 */

#ifndef MESSD_H
#define MESSD_H

#include <stdbool.h>
#include <cutesynth.h>

#define NUM_DIVISION_VALUES 10
#define MS_PER_MINUTE 60000

 /**
  * messd_t: Module's main data structure
  */
typedef struct messd
{
    t_phase_locked_loop p_locked_loop;

    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;

    float lastRootClockPhase;
    float lastScaledClockPhase;
    uint8_t beatCounter;

    uint8_t scaledBeatCounter;

    bool invertNeedsReset;
    bool modulationNeedsReset;

    uint16_t tempoMultiply;
    uint16_t tempoDivide;
    uint16_t previousTempoMultiply;
    uint16_t previousTempoDivide;
} messd_t;

typedef struct messd_ins
{
    double tempo;
    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;
    double ext_clock;

    bool ext_clock_connected; // true if an external clock is connected
    bool metricModulation;
    bool latchToDownbeat;
    bool invert;
    bool isRoundTrip;
    bool reset;

    uint8_t wrap;
    double phase;
    double pulseWidth;

    double delta;
} messd_ins_t;

typedef struct messd_outs
{
    bool downbeat;
    bool beat;
    bool subdivision;
    bool phase;

    bool invert;
    bool modulate; // High when modulation trigger is active
    uint8_t subdivisions; // The number of subdivisions in use, after modulation
} messd_outs_t;

/**
 * Initialize the 'messd' struct
 * */
void MS_init(messd_t *self);

/**
 * Frees the 'messd' struct
 * */
void MS_destroy(messd_t *self);

/**
 * Provide a hint for the wavelength of a clock cycle
 */
void MS_clock_wavelength_hint(messd_t *self, float hint);

/**
 * Processes the incoming CV data in main.cpp
 */
void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs);

#endif /* MESSD_H */
