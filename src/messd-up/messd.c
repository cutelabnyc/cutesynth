#include "messd.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void MS_init(messd_t *self)
{
    phasor_init(&self->p_clock);
    phasor_init(&self->p_downbeat);
    phasor_init(&self->p_subdivision);

    edge_init(&self->phaseEdge);
    edge_init(&self->downEdge);
    edge_init(&self->subEdge);

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

    double phasor = 0;

    // Check for metric modulation
    if (ins[METRIC_MODULATION])
    {
        tempo = (tempo * self->subdivision) / self->downbeat;
    }

    // Calculate initial tempo tick
    phasor = phasor_step(&self->p_clock, tempo);
    edge_process(&self->phaseEdge, &phasor, &outs[CLOCK_OUT]);

    // If beats per measure changes, check for tempo tick to latch a new downbeat onto
    if (outs[CLOCK_OUT])
    {
        self->downbeat = ((ins[DOWNBEAT_IN] / 1024.0) * NUM_DIVISION_VALUES) + 1;
    }

    // Calculate downbeat
    phasor = phasor_step(&self->p_downbeat, tempo / self->downbeat);
    edge_process(&self->downEdge, &phasor, &outs[DOWNBEAT_OUT]);


    // If subdivisions number changes, check for downbeat edge to latch new subdivision onto
    if (outs[DOWNBEAT_OUT])
    {
        self->subdivision = ((ins[SUBDIVISION_IN] / 1024.0) * NUM_DIVISION_VALUES) + 1;
    }

    // Calculate subdivisions
    phasor = phasor_step(&self->p_subdivision, (tempo * self->subdivision) / self->downbeat);
    edge_process(&self->subEdge, &phasor, &outs[SUBDIVISION_OUT]);

    // Process phased output
    self->theta = (ins[PHASE_IN] / 1024);
    outs[PHASE_OUT] = fmod(phasor + self->theta, 1);
}
