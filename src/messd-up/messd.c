#include "messd.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void MS_init(messd_t *self)
{
    phasor_init(&self->p_clock);
    phasor_init(&self->p_downbeat);
    phasor_init(&self->p_subdivision);

    self->downbeat = 1;
    self->subdivision = 1;
    self->theta = 0.0f;
}

void MS_destroy(messd_t *self)
{
}

void MS_process(messd_t *self, double *ins, double *outs)
{
    // Calculate clock based on tempo in
    float tempo = (ins[CLOCK_KNOB] / (float)UINT16_MAX);

    double clock = 0;
    double downbeat = 0;
    double subdivision = 0;
    double phasor = 0;

    // Check for metric modulation
    if (ins[METRIC_MODULATION])
    {
        tempo = (tempo * self->subdivision) / (float)self->downbeat;
    }

    // Calculate initial tempo tick
    clock = phasor_step(&self->p_clock, tempo);
    outs[CLOCK_OUT] = (clock < 0.5) ? 1 : 0;

    // If beats per measure changes, check for tempo tick to latch a new downbeat onto
    if (outs[CLOCK_OUT])
    {
        self->theta = (ins[PHASE_IN] / 1024.0f);

        // self->downbeat = ((ins[DOWNBEAT_IN] / 1024.0) * NUM_DIVISION_VALUES) + 1;
        self->downbeat = 3;
    }

    // Calculate downbeat
    downbeat = phasor_step(&self->p_downbeat, tempo / self->downbeat);
    outs[DOWNBEAT_OUT] = (downbeat < 0.5) ? 1 : 0;

    // If subdivisions number changes, check for downbeat edge to latch new subdivision onto
    if (outs[DOWNBEAT_OUT])
    {
        // self->subdivision = ((ins[SUBDIVISION_IN] / 1024.0) * NUM_DIVISION_VALUES) + 1;
        self->subdivision = 7;
    }

    // Calculate subdivisions
    subdivision = phasor_step(&self->p_subdivision, (tempo * self->subdivision) / self->downbeat);
    outs[SUBDIVISION_OUT] = (subdivision < 0.5) ? 1 : 0;

    // Process phased output
    phasor = fmod(clock + self->theta, 1.0f);
    outs[PHASE_OUT] = (phasor < 0.5) ? 1 : 0;
}
