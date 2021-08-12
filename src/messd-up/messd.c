#include "messd.h"
#include <stdlib.h>
#include <math.h>

void MS_init(messd_t *self)
{
    phasor_init(&self->p_clock);
    phasor_init(&self->p_downbeat);
    phasor_init(&self->p_subdivision);

    edge_init(&self->downEdge);
    edge_init(&self->subEdge);

    self->downbeat = 1;
    self->subdivision = 1;
    self->theta = 0.0f;
}

void MS_destroy(messd_t *self)
{
}

void MS_process(messd_t *self, uint16_t *args)
{
    uint16_t CLOCK_in, CLOCK_out, DOWNBEAT_in, DOWNBEAT_out;
    uint16_t SUBDIVISION_in, SUBDIVISION_out, PHASE_in, PHASE_out, METRIC_MODULATION_in;

    CLOCK_in = args[0];
    CLOCK_out = args[1];
    DOWNBEAT_in = args[2];
    DOWNBEAT_out = args[3];
    SUBDIVISION_in = args[4];
    SUBDIVISION_out = args[5];
    PHASE_in = args[6];
    PHASE_out = args[7];
    METRIC_MODULATION_in = args[8];

    // Calculate clock based on tempo in
    float tempo = ((float)CLOCK_in / (float)UINT16_MAX);
    float phasor = 0;
    float dutyCycle = 0.5f;

    // Check for metric modulation
    if (METRIC_MODULATION_in)
    {
        tempo = (tempo * (float)self->subdivision) / (float)self->downbeat;
    }

    phasor = phasor_step(&self->p_clock, tempo);
    CLOCK_out = (phasor <= 0.5);

    // Check for clock edges to latch downbeat onto
    uint16_t postEdge;
    edge_process(&self->downEdge, CLOCK_out, &postEdge);

    if (CLOCK_out)
    {
        self->downbeat = ((float)DOWNBEAT_in / 1024.0) * NUM_DIVISION_VALUES;
    }

    // Process downbeat tempo
    DOWNBEAT_out = phasor_step(&self->p_downbeat, tempo / (float)self->downbeat);

    // Check for downbeat edges to latch subdivisions onto
    edge_process(&self->subEdge, DOWNBEAT_out, &postEdge);

    if (postEdge)
    {
        self->subdivision = ((float)SUBDIVISION_in / 1024.0) * NUM_DIVISION_VALUES;
    }

    // Process subdivisions
    SUBDIVISION_out = phasor_step(&self->p_subdivision, (tempo * (float)self->subdivision) / (float)self->downbeat);

    // Process phased output
    self->theta = ((float)PHASE_in / 1024);
    PHASE_out = fmod(phasor + self->theta, 2) > 1;
}
