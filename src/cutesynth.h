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

#define INPUT 0x0
#define DIGITAL 0x0
#define OUTPUT  0x1
#define ANALOG 0x1

#define A0 14U
#define A1 15U
#define A2 16U
#define A3 17U
#define A4 18U
#define A5 19U
#define A6 20U
#define A7 21U

#define NUM_PIN_PARAMS 3

/**
 * Function pointers to main init/process
 * functions, initializes in hardware files
 */
#if MESSD_UP

#include "messd-up/messd.h"

#define NUM_INPUTS 5
#define NUM_OUTPUTS 4
#define NUM_ARGS (NUM_INPUTS + NUM_OUTPUTS)

messd_t messd;

// Input table to be turned into pin_t* in the devkit
uint8_t input_table[NUM_INPUTS][NUM_PIN_PARAMS] = {
    {A6, INPUT, ANALOG}, // Clock In
    {A3, INPUT, ANALOG}, // Downbeat in
    {A4, INPUT, ANALOG}, // Subdivision in
    {A7, INPUT, ANALOG}, // Phase in
    {7, INPUT, DIGITAL}   // Metric Modulation
};

// Output table to be turned into pin_t* in the devkit
uint8_t output_table[NUM_OUTPUTS][NUM_PIN_PARAMS] = {
    {4, OUTPUT, DIGITAL},  // Clock out,
    {12, OUTPUT, DIGITAL}, // Downbeat out
    {10, OUTPUT, DIGITAL}, // Subdivision out,
    {8, OUTPUT, DIGITAL}  // Phase out
};

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

