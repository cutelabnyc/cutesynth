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
    phasor_t internalClock;

    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;

    uint8_t lastClock;
    float measuredPeriod;
    float msSinceLastLeadingEdge;
    float lastRootClockPhase;
    float lastScaledClockPhase;
    uint8_t beatCounter;

    uint8_t scaledBeatCounter;

    bool lastModulationSignal;
    bool lastModulationSwitch;
    bool inRoundTripModulation;
    bool invertNeedsReset;
    bool modulationPending;

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
    uint8_t truncation;
    double ext_clock;

    bool ext_clock_connected; // true if an external clock is connected
    bool modulationSignal; // continuous modulation signal
    bool modulationSwitch; // modulation pushbutton
    bool latchChangesToDownbeat;
    bool latchModulationToDownbeat;
    bool invert;
    bool isRoundTrip;
    bool reset;

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
    bool truncate;
    bool eom;

    bool invert;
    bool modulationPending; // High when modulation trigger is active
    uint8_t subdivisions; // The number of subdivisions in use, after modulation
	double scaledTempo; // Output tempo after scaling
    double measuredTempo; // Measured tempo of the external clock
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
 * Set the numerator (multiplier) of the output clock frequency
 */
void MS_set_output_multiplier(messd_t *self, int multiplier);

/**
 * Set the dividend (divisor) of the output clock frequency
 */
void MS_set_output_divisor(messd_t *self, int divisor);

/**
 * Processes the incoming CV data in main.cpp
 */
void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs);

#endif /* MESSD_H */
