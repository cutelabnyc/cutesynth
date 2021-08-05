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


// TODO: Use function pointers
void IT_init();
void IT_process();

#if MESSD_UP
#include "messd-up/messd.h"

void IT_init()
{
    MS_init(&messd);
}

void IT_process()
{
    MS_process(&messd,
        &CLOCK_in,
        &CLOCK_out,
        &DOWNBEAT_in,
        &DOWNBEAT_out,
        &SUBDIVISION_in,
        &SUBDIVISION_out,
        &PHASE_in,
        &PHASE_out,
        METRIC_MODULATION_in);
}

#elif MISSED_OPPORTUNITIES
#include "missed-opportunities/opportunity.h"
#elif UEXKULL
#include "uexkull/uexkull.h"
#endif

