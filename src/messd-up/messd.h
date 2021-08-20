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

    t_edge phaseEdge;
    t_edge subEdge;
    t_edge downEdge;

    uint8_t downbeat;
    uint8_t subdivision;
    float theta;
} messd_t;

typedef enum {
    CLOCK_KNOB,
    CLOCK_OUT,
    DOWNBEAT_IN,
    DOWNBEAT_OUT,
    SUBDIVISION_IN,
    SUBDIVISION_OUT,
    PHASE_IN,
    PHASE_OUT,
    METRIC_MODULATION
} params_t;

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
void MS_process(messd_t *self, double **ins, double **outs);

#endif /* MESSD_H */
