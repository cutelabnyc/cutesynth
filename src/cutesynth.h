#include "op/autopulse.h"
#include "op/counter.h"
#include "op/edge.h"
#include "op/gate.h"
#include "op/random.h"
#include "op/thresh.h"
#include "dsp/oscillator.h"
#include "dsp/lut/lut.h"
#include "op/series.h"
#include "dsp/bank.h"
#include "op/avg.h"
#include "op/fb-smooth.h"
#include "op/phasor.h"

/**
 * Function pointers to main init/process
 * functions, initializes in hardware files
 */

void(*MAIN_init_f)(void *);
void(*MAIN_process_f)(void *, uint16_t *);

typedef struct pin
{
    uint8_t pin;
    bool type;
} pin_t;

typedef pin_t *GPIO_t;


#if MESSD_UP
#include "messd-up/messd.h"

#define NUM_ARGS 9

// Define init, process, arg array size
(*MAIN_init_f)(void *) = &MS_init;
(*MAIN_process_f)(void *, uint16_t *) = &MS_process;

GPIO_t GPIO[NUM_ARGS] = { { A6, INPUT }, // CLOCK_KNOB
                            { 4, OUTPUT }, // CLOCK_OUT
                            { A3, INPUT }, // DOWNBEAT_IN
                            { 12, OUTPUT }, // DOWNBEAT_OUT
                            { A4, INPUT }, // SUBDIVISION_IN
                            { 10, OUTPUT }, // SUBDIVISION_OUT
                            { A7, INPUT }, // PHASE_IN
                            { 8, OUTPUT }, // PHASE_OUT
                            { 7, INPUT } // METRIC_MODULATION
}

#elif MISSED_OPPORTUNITIES
#include "missed-opportunities/opportunity.h"
#elif UEXKULL
#include "uexkull/uexkull.h"
#endif

