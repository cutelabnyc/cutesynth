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


#if MESSD_UP
#include "messd-up/messd.h"


#define NUM_INPUTS 5
#define NUM_OUTPUTS 4
#define NUM_ARGS (NUM_INPUTS + NUM_OUTPUTS)

 // typedef void (*MAIN_init_f)(messd_t *);
 // typedef void (*MAIN_process_f)(messd_t *, uint16_t *);

 // Define init, process, arg array size
// MAIN_init_f = MS_init;
// MAIN_process_f = MS_process;

#elif MISSED_OPPORTUNITIES
#include "missed-opportunities/opportunity.h"
#elif UEXKULL
#include "uexkull/uexkull.h"
#endif

