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
#include "../op/phase-locked-loop.h"
#include "../op/phasor.h"
#include "../op/edge.h"

#define NUM_DIVISION_VALUES 10
#define MS_PER_MINUTE 60000

 /**
  * messd_t: Module's main data structure
  */
typedef struct messd
{
    phasor_t p_clock;
    t_phase_locked_loop p_locked_loop;

    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;
    uint8_t beatKonducta;
    double lastBeatPhase;

    bool invertNeedsReset;
    bool modulationNeedsReset;

    double tempoScale;
    double previousTempoScale;
} messd_t;

typedef struct messd_ins
{
    double tempo;
    uint8_t beatsPerMeasure;
    uint8_t subdivisionsPerMeasure;
    double ext_clock;

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
    bool modulate;
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
