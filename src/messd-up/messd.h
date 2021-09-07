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
#include "../op/phasor.h"
#include "../op/edge.h"

#define NUM_DIVISION_VALUES 10

 /**
  * messd_t: Module's main data structure
  */
typedef struct messd
{
    phasor_t p_clock;
    phasor_t p_downbeat;
    phasor_t p_subdivision;

    uint8_t downbeat;
    uint8_t subdivision;
    
    float truncate;
    float pulse_width;
    float theta;

    bool downbeat_flag;
    bool subdivision_flag;

} messd_t;

typedef enum {
    TEMPO_KNOB,
    DOWNBEAT_IN,
    SUBDIVISION_IN,
    PHASE_IN,
    METRIC_MODULATION,
    BEAT_LATCH,
    TRUNCATE,
    INVERT,
    PULSE_WIDTH
} ins_t;

typedef enum {
    BEAT_OUT,
    DOWNBEAT_OUT,
    SUBDIVISION_OUT,
    PHASE_OUT,
} outs_t;

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
void MS_process(messd_t *self, double *ins, double *outs);

#endif /* MESSD_H */
