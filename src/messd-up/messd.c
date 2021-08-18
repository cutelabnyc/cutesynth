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

void MS_process(messd_t *self,
    double *clock_in,
    double *clock_out,
    double *downbeat_in,
    double *downbeat_out,
    double *subdivision_in,
    double *subdivision_out,
    double *phase_in,
    double *phase_out,
    bool metric_modulation)
{
    // Calculate clock based on tempo in
    float tempo = (*clock_in / (float)UINT16_MAX);
    double phasor = 0;
    float dutyCycle = 0.5f;

    // Check for metric modulation
    if (metric_modulation)
    {
        tempo = (tempo * self->subdivision) / self->downbeat;
    }

    // Calculate initial tempo tick
    phasor = phasor_step(&self->p_clock, tempo);
    edge_process(&self->phaseEdge, &phasor, clock_out);
    
    // If beats per measure changes, check for tempo tick to latch a new downbeat onto
    if (*clock_out)
    {
        self->downbeat = (*downbeat_in / 1024.0) * NUM_DIVISION_VALUES;
    }
    
    // Calculate downbeat
    phasor = phasor_step(&self->p_downbeat, tempo / self->downbeat);
    edge_process(&self->downEdge, &phasor, downbeat_out);
    
    
    // If subdivisions number changes, check for downbeat edge to latch new subdivision onto
    if (*downbeat_out)
    {
        self->subdivision = (*subdivision_in / 1024.0) * NUM_DIVISION_VALUES;
    }

    // Calculate subdivisions
    phasor = phasor_step(&self->p_subdivision, (tempo * self->subdivision) / self->downbeat);
    edge_process(&self->subEdge, &phasor, subdivision_out);

    // Process phased output
    self->theta = (*phase_in / 1024);
    *phase_out = fmod(phasor + self->theta, 1) > 1;
}
