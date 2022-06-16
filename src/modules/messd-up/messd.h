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

// Uncomment to track the input clock period internally. If you
// can measure the clock externally, pass it to cheatedMeasuredPeriod
// and leave this commented out to save on program memory
// #define TRACK_INPUT_CLOCK_PERIOD

 /**
  * messd_t: Module's main data structure
  */
typedef struct messd
{
    phasor_t internalClock;

    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;

    uint8_t lastClock;
    float measuredPeriod;
    float lastRootClockPhase;
    float lastScaledClockPhase;
    float measuredTempo;
    uint8_t beatCounter;

#ifdef TRACK_INPUT_CLOCK_PERIOD
    float msSinceLastLeadingEdge;
#endif

    uint8_t scaledBeatCounter;

    bool lastModulationSignal;
    bool lastModulationSwitch;
    bool inRoundTripModulation;
    bool invertNeedsReset;
    bool modulationPending;
    bool resetPending;
    bool modulateOnEdgeEnabled;

    uint16_t tempoMultiply;
    uint16_t tempoDivide;
    uint16_t previousTempoMultiply;
    uint16_t previousTempoDivide;
} messd_t;

typedef struct messd_ins
{
    float tempo;
    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;
    float truncation; // normalized to the number of beats
    float ext_clock;

    bool modulationSignal; // continuous modulation signal
    bool modulationSwitch; // modulation pushbutton
    bool latchBeatChangesToDownbeat;
    bool latchDivChangesToDownbeat;
    bool latchModulationToDownbeat;
    bool invert;
    bool isRoundTrip;
    bool reset;

    float phase;
    float pulseWidth;

    float delta;
    unsigned long cheatedMeasuredPeriod;

#ifdef TRACK_INPUT_CLOCK_PERIOD
    unsigned long microsClockOffset;
#endif
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
    bool resetPending; // High when the next modulation is resetting
    bool inRoundTripModulation; // High when a round trip modulation is active
    uint8_t subdivisions; // The number of subdivisions in use, after modulation
	float scaledTempo; // Output tempo after scaling
    float measuredTempo; // Measured tempo of the external clock
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
 * Processes the incoming CV data in main.cpp
 */
void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs);

#endif /* MESSD_H */
