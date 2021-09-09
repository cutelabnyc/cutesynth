#include "messd.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void MS_init(messd_t *self)
{
    phasor_init(&self->p_clock);

    self->beatsPerMeasure = 1;
    self->subdivisionsPerMeasure = 0;

    self->beatKonducta = 0;
    self->lastBeatPhase = 1;
    self->tempoScale = 1;
}

void MS_destroy(messd_t *self)
{
}

static void _MS_updateParams(messd_t *self, messd_ins_t *ins)
{
    // Check for metric modulation
    // NOTE: come back to this!
    // if (ins->metricModulation)
    // {
    //     self->tempoScale *= (float)ins->subdivisionsPerMeasure / (float)ins->beatsPerMeasure;
    // }

    self->beatsPerMeasure = ins->beatsPerMeasure;
    self->subdivisionsPerMeasure = ins->subdivisionsPerMeasure;
}

void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    // Calculate clock based on tempo in
    float phaseDelta = ((ins->tempo * self->tempoScale) * ins->delta) / MS_PER_MINUTE;

    double beatPhase = 0;
    double measurePhase = 0;
    double subdivision = 0;
    double phasor = 0;

    // Calculate initial tempo tick
    beatPhase = phasor_step(&self->p_clock, phaseDelta);
    outs->beat = beatPhase < ins->pulseWidth;

    if (self->lastBeatPhase > beatPhase)
    {
        self->beatKonducta++;
        if (!(ins->latchToDownbeat && (self->beatKonducta % self->beatsPerMeasure != 0)))
        {
            _MS_updateParams(self, ins);
        }
    }

    // Calculate downbeat
    measurePhase = beatPhase + (self->beatKonducta % self->beatsPerMeasure);
    measurePhase /= self->beatsPerMeasure;
    outs->downbeat = measurePhase < ins->pulseWidth;

    // Calculate subdivisions
    subdivision = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);
    outs->subdivision = subdivision < ins->pulseWidth;

    // Process phased output
    phasor = fmod(beatPhase + ins->phase, 1.0f);
    outs->phase = phasor < ins->pulseWidth;

    self->lastBeatPhase = beatPhase;

}
