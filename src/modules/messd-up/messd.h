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
    phasor_t internalClock; // This is a phasor (0-1 ramp) synchronized to the input clock
    uint8_t lastClock; // Previous value of the input clock
    float measuredPeriod; // Recorded time between two input clock leading edges
    float measuredTempo; // Time between two input clocks as beats per minute

    uint8_t beatsPerMeasure; // Current number of beats to a measure
    uint8_t subdivisionsPerMeasure; // Current number of subdivisions to a measure
    uint16_t tempoMultiply; // Together with tempoDivide, defines the duration ratio
                            // of one stretched beat to one root clock beat.
    uint16_t tempoDivide;

    float rootClockPhase; // Previous phase of the internal phasor
    float scaledClockPhase; // Previous phase of the internal phasor after stretching
    float rootClockPhaseOffset; // Offset in <unstretched-beats>.<phase> of the internal clock
                                // to maintain alignent in free mode

    // Stuff needed for round-trip mode
    // These values are recorded whenever the module enters a round-trip modulation
    uint16_t homeTempoMultiply;
    uint16_t homeTempoDivide;
    uint8_t homeSubdivisionsPerMeasure;
    uint8_t homeBeatsPerMeasure;

    // Stuff needed for latch mode
    uint32_t rootBeatsSinceModulation;  // Counts up from 0 the number of root clock events since the last
                                        // modulation. Needed to calculate the number of root clock events
                                        // until the root clock and stretched clock are in phase on a
                                        // stretched downbeat
    uint16_t countdown;                 // Root clock events remaining until the clocks are in sync
    uint16_t memoizedCountdownMax;
    uint8_t memoizedBeatsPerMeasure;
    bool isLatching;

#ifdef TRACK_INPUT_CLOCK_PERIOD
    float msSinceLastLeadingEdge;
#endif

    uint8_t rootBeatCounter;    // Beat count for the input clock
    uint8_t scaledBeatCounter;  // Beat count for the scaled clock

    bool lastModulationSignal;  // Needed to detect leading edges on the modulation signal
    bool lastModulationSwitch;  // Needed to detect leading edges on the modulation switch
    bool inRoundTripModulation;
    bool invertNeedsReset;
    bool modulationPending;
    bool resetPending;
    bool modulateOnEdgeEnabled;
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
    bool resetBeatCount;

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
    uint16_t countdown; // The number of measures remaining in the countdown until the pending modulation will be triggered
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
