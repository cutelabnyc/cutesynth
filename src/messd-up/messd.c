#include "messd.h"

#include <math.h>

void MS_init(messd_t *self)
{
    phasor_init(&self->p_clock);
    phase_locked_loop_init(&self->p_locked_loop);

    self->beatsPerMeasure = 1;
    self->subdivisionsPerMeasure = 0;

    self->beatKonducta = 0;
    self->lastBeatPhase = 1;
    self->tempoScale = 1;
    self->previousTempoScale = 1;

    self->invertNeedsReset = false;
    self->modulationNeedsReset = false;
}

void MS_destroy(messd_t *self)
{
}

static void _MS_handleLatch(messd_t *self, messd_ins_t *ins)
{
    // Update the beats and the subdivisions
    self->beatsPerMeasure = ins->beatsPerMeasure;
    self->subdivisionsPerMeasure = ins->subdivisionsPerMeasure;

    if (ins->reset)
    {
        self->tempoScale = 1;
    }

    // Check for metric modulation
    if (ins->metricModulation && !self->modulationNeedsReset)
    {
        if (ins->isRoundTrip)
        {
            self->previousTempoScale = self->tempoScale;
        }

        self->tempoScale *= (float)ins->subdivisionsPerMeasure / (float)ins->beatsPerMeasure;
        self->modulationNeedsReset = true;
        self->subdivisionsPerMeasure = self->beatsPerMeasure;

        // Set subdivisions equal to beats upon metric modulation
        ins->subdivisionsPerMeasure = self->beatsPerMeasure;
    }
    else if (ins->isRoundTrip) {
        // Check for roundtrip mode and modulate
        if (!ins->metricModulation)
        {
            self->tempoScale = self->previousTempoScale;
        }
    }

    // Check to apply an invert
    if (ins->invert && !self->invertNeedsReset)
    {
        ins->invert = false;

        self->beatsPerMeasure = ins->subdivisionsPerMeasure;
        self->subdivisionsPerMeasure = ins->beatsPerMeasure;
        self->invertNeedsReset = true;

        ins->subdivisionsPerMeasure = self->subdivisionsPerMeasure;
        ins->beatsPerMeasure = self->beatsPerMeasure;
    }
}

static void _MS_handleExternalClock(messd_t *self, double reference)
{

}

void MS_clock_wavelength_hint(messd_t *self, float hint)
{
	phase_locked_loop_hint(&self->p_locked_loop, hint);
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
            _MS_handleLatch(self, ins);
        }
    }

    // Reset metric modulation
    if (!ins->metricModulation)
    {
        self->modulationNeedsReset = false;
    }

    if (!ins->invert)
    {
        self->invertNeedsReset = false;
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

    // Test
    uint16_t pll_in = ins->ext_clock < 0.5;
    outs->test_out = phase_locked_loop_process(&self->p_locked_loop, &pll_in);
}
