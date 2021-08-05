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

    t_edge subEdge;
    t_edge downEdge;

    uint8_t downbeat;
    uint8_t subdivision;
    float theta;
} messd_t;


messd_t messd;

uint16_t CLOCK_in;
uint16_t CLOCK_out;
uint16_t DOWNBEAT_in;
uint16_t DOWNBEAT_out;
uint16_t SUBDIVISION_in;
uint16_t SUBDIVISION_out;
uint16_t PHASE_in;
uint16_t PHASE_out;
bool METRIC_MODULATION_in;

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
    uint16_t *clock_out,
    uint16_t *downbeat_in,
    uint16_t *downbeat_out,
    uint16_t *subdivision_in,
    uint16_t *subdivision_out,
    uint16_t *phase_in,
    uint16_t *phase_out,
    bool metric_modulation);


#endif /* MESSD_H */
