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

GPIO_t GPIO[NUM_ARGS] = { { A6, INPUT },
                            { 4, OUTPUT },
                            { A3, INPUT },
                            { 12, OUTPUT },
                            { A4, INPUT },
                            { 10, OUTPUT },
                            { A7, INPUT },
                            { 8, OUTPUT },
                            { 7, INPUT }
}

#elif MISSED_OPPORTUNITIES
#include "missed-opportunities/opportunity.h"
#elif UEXKULL
#include "uexkull/uexkull.h"
#endif

