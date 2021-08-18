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
#include "../../src/cutesynth.h"

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

    float downbeat;
    float subdivision;
    float theta;
} messd_t;

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
 *
 * TODO: Make a struct for I/O to simplify this?
 */
void MS_process(messd_t *self,
    uint16_t *clock_in,
    double *clock_out,
    uint16_t *downbeat_in,
    double *downbeat_out,
    uint16_t *subdivision_in,
    double *subdivision_out,
    uint16_t *phase_in,
    double *phase_out,
    bool metric_modulation);


#endif /* MESSD_H */
