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
    
    self->truncate = 0;

    self->downbeat_flag = true;
    self->subdivision_flag = true;
}

void MS_destroy(messd_t *self)
{
}

static void _MS_scale(double *ins)
{
    ins[TEMPO_KNOB] = (ins[TEMPO_KNOB] > 0 ? ins[TEMPO_KNOB] : 1);
    ins[DOWNBEAT_IN] = (ins[DOWNBEAT_IN] > 0 ? ins[DOWNBEAT_IN] : 1);
    ins[SUBDIVISION_IN] = (ins[SUBDIVISION_IN] > 0 ? ins[SUBDIVISION_IN] : 1);
    ins[PHASE_IN] = (ins[PHASE_IN] <= 1 && ins[PHASE_IN] >= 0  ? ins[PHASE_IN] : 0);
    ins[PULSE_WIDTH] = (ins[PULSE_WIDTH] < 1 && ins[PULSE_WIDTH] > 0  ? ins[PULSE_WIDTH] : 0.5);
}

void MS_process(messd_t *self, double *ins, double *outs)
{
    // Calculate clock based on tempo in
    float tempo = (ins[TEMPO_KNOB] / (float)UINT16_MAX);
//    bool latch = (ins[BEAT_LATCH] ? outs[DOWNBEAT_OUT] : outs[BEAT_OUT]);
    bool latch = outs[DOWNBEAT_OUT];

    double beat = 0;
    double downbeat = 0;
    double subdivision = 0;
    double phasor = 0;
        
    _MS_scale(ins);
        
    // Check for metric modulation
    if (ins[METRIC_MODULATION])
    {
        tempo = (tempo * self->subdivision) / (float)self->downbeat;
    }
    
    // Check param changes
    if (latch)
    {
        // If beats per measure changes, check for tempo tick to latch a new downbeat onto
        if (self->downbeat_flag)
        {
            self->theta = ins[PHASE_IN];
            self->downbeat = ins[DOWNBEAT_IN];
            self->downbeat_flag = false;
        }
        
        // If subdivisions number changes, check for downbeat edge to latch new subdivision onto
        if (self->subdivision_flag)
        {
            self->subdivision = ins[SUBDIVISION_IN];
            self->subdivision_flag = false;
        }
    }
    else {
        self->downbeat_flag = true;
        self->subdivision_flag = true;
    }
    
    // Calculate initial tempo tick
    beat = phasor_step(&self->p_clock, tempo);
    outs[BEAT_OUT] = (beat < ins[PULSE_WIDTH]) ? 1 : 0;

    // Calculate downbeat
    downbeat = phasor_step(&self->p_downbeat, tempo / self->downbeat);
    outs[DOWNBEAT_OUT] = (downbeat < ins[PULSE_WIDTH]) ? 1 : 0;

    // Calculate subdivisions
    subdivision = phasor_step(&self->p_subdivision, (tempo * self->subdivision) / self->downbeat);
    outs[SUBDIVISION_OUT] = (subdivision < ins[PULSE_WIDTH]) ? 1 : 0;

    // Process phased output
    phasor = fmod(beat + self->theta, 1.0f);
    outs[PHASE_OUT] = (phasor < ins[PULSE_WIDTH]) ? 1 : 0;
}
