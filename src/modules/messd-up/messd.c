#include "messd.h"

#include <math.h>

void MS_init(messd_t *self)
{
    phase_locked_loop_init(&self->p_locked_loop);

    self->beatsPerMeasure = 1;
    self->subdivisionsPerMeasure = 0;
    self->lastRootClockPhase = 1;
    self->lastScaledClockPhase = 1;

    self->beatCounter = 0;
    self->scaledBeatCounter = 0;
    self->tempoMultiply = 1;
    self->tempoDivide = 1;
    self->previousTempoMultiply = 1;
    self->previousTempoDivide = 1;

    self->invertNeedsReset = false;
    self->modulationNeedsReset = false;
}

void MS_destroy(messd_t *self)
{
}

static void reduceFraction(uint16_t n_in, uint16_t d_in, uint16_t *n_out, uint16_t *d_out)
{
    *n_out = n_in;
    *d_out = d_in;
    uint16_t counter = n_in < d_in ? n_in : d_in;
    while (counter > 1) {
        if (*n_out % counter == 0 && *d_out % counter == 0) {
            *n_out /= counter;
            *d_out /= counter;
            counter = *n_out < *d_out ? *n_out : *d_out;
        } else {
            counter--;
        }
    }
}

static void _MS_handleModulationLatch(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    // Check for metric modulation
    if (ins->metricModulation && !self->modulationNeedsReset)
    {
        if (ins->isRoundTrip)
        {
            self->previousTempoMultiply = self->tempoMultiply;
            self->previousTempoDivide = self->tempoDivide;
        }

        self->tempoMultiply *= ins->subdivisionsPerMeasure;
        self->tempoDivide *= ins->beatsPerMeasure;
        reduceFraction(self->tempoMultiply, self->tempoDivide, &self->tempoMultiply, &self->tempoDivide);
        self->modulationNeedsReset = true;
        self->subdivisionsPerMeasure = self->beatsPerMeasure;

        // Set subdivisions equal to beats upon metric modulation
        ins->subdivisionsPerMeasure = self->beatsPerMeasure;
        outs->modulate = true;
    }
    else if (ins->isRoundTrip) {
        // Check for roundtrip mode and modulate
        if (!ins->metricModulation)
        {
            self->tempoMultiply = self->previousTempoMultiply;
            self->tempoDivide = self->previousTempoDivide;
        }
        outs->modulate = true;
    }
}

static void _MS_handleLatch(messd_t *self, messd_ins_t *ins)
{
    // Update the beats and the subdivisions
    self->beatsPerMeasure = ins->beatsPerMeasure;
    self->subdivisionsPerMeasure = ins->subdivisionsPerMeasure;

    if (ins->reset)
    {
        self->tempoMultiply = 1;
        self->tempoDivide = 1;
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

void MS_clock_wavelength_hint(messd_t *self, float hint)
{
	phase_locked_loop_hint(&self->p_locked_loop, hint);
}

void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float rootClockPhase = 0;
    float scaledClockPhase = 0;
    double measurePhase = 0;
    double measurePhaseFloor = 0;
    double measurePhaseCeiling = 0;
    double subdivision = 0;
    double phasor = 0;
    int pll_in = -1;

    outs->modulate = false;

    // ==== Root clock calculations

    // Calculate clock based on tempo in
    float phaseDelta = (ins->tempo * ins->delta) / MS_PER_MINUTE;

    // Decide whether or not to use it based on input
    if (ins->ext_clock_connected) {
        pll_in = ins->ext_clock > 0.5; // clock in is a pulse train
    } else {
        phase_locked_loop_set_frequency(&self->p_locked_loop, phaseDelta);
    }
    rootClockPhase = phase_locked_loop_process(&self->p_locked_loop, &pll_in);
    outs->measuredTempo = self->p_locked_loop._frequency * MS_PER_MINUTE / ins->delta;

    // Count beats on the clock
    if (self->lastRootClockPhase > rootClockPhase) {
        self->beatCounter = (self->beatCounter + 1) % self->tempoDivide;
    }
    self->lastRootClockPhase = rootClockPhase;

    // Multiply the clock to get the final phase
    scaledClockPhase = rootClockPhase + self->beatCounter;
    scaledClockPhase *= self->tempoMultiply;
    scaledClockPhase /= self->tempoDivide;
	scaledClockPhase = fmod(scaledClockPhase, 1.0f);

    // ==== Output calculation

    outs->beat = scaledClockPhase < ins->pulseWidth;

    // Latch to beat events
    if (self->lastScaledClockPhase > scaledClockPhase)
    {
        self->scaledBeatCounter = (self->scaledBeatCounter + 1) % self->beatsPerMeasure;

        // Handle changes
        if (!(ins->latchChangesToDownbeat && self->scaledBeatCounter != 0))
        {
            _MS_handleLatch(self, ins);
        }

        // Then handle modulation changes
        if (!(ins->latchModulationToDownbeat && self->scaledBeatCounter != 0))
        {
            _MS_handleModulationLatch(self, ins, outs);
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
    measurePhase = scaledClockPhase + self->scaledBeatCounter;
    measurePhase /= self->beatsPerMeasure;
    outs->downbeat = measurePhase < ins->pulseWidth;

    // Calculate subdivisions
    if (ins->truncation > 0) {
        uint8_t truncatedBeatCount = (self->scaledBeatCounter / ins->truncation) * ins->truncation;
        float measurePhaseInTrunc = fmodf(scaledClockPhase + self->scaledBeatCounter, ins->truncation) / self->beatsPerMeasure;
        float measureOffset = ((float) truncatedBeatCount) / self->beatsPerMeasure;

        // Normally the subdivision phase is just the fractional component of the subdivision count.
        // This changes however if we would overlap with the next truncation, or the end of the measure
        float subdivisionProgress = measurePhaseInTrunc * self->subdivisionsPerMeasure;
		float nextSubdivisionProgress = ceil(subdivisionProgress);
		float subdivisionFrac = 1.0 / self->subdivisionsPerMeasure;

        // Next subdivision would go past the end of the measure
        if (
            truncatedBeatCount / ins->truncation == ins->truncation - 1 &&
            nextSubdivisionProgress / self->subdivisionsPerMeasure + measureOffset > 1.0
        ) {
            subdivision = fmodf(subdivisionProgress, 1.0f);
			subdivision /= (1.0 - (measureOffset + floor(subdivisionProgress) / self->subdivisionsPerMeasure));
			subdivision *= subdivisionFrac;
        }

        // Next subdivision would go past the next truncation
        else if (
            nextSubdivisionProgress / self->subdivisionsPerMeasure > ((float) ins->truncation) / self->beatsPerMeasure
        ) {
            subdivision = fmodf(subdivisionProgress, 1.0f);
            subdivision /= ((float) ins->truncation) / self->beatsPerMeasure - floor(subdivisionProgress) / self->subdivisionsPerMeasure;
			subdivision *= subdivisionFrac;
        }

        // "Normal" situation
        else {
            subdivision = fmodf(subdivisionProgress, 1.0f);
        }

        // In terms of phase
        outs->subdivision = subdivision < ins->pulseWidth;
    } else {
        subdivision = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);
        outs->subdivision = subdivision < ins->pulseWidth;
    }

    // Process phased output
    phasor = fmod(scaledClockPhase + ins->phase, 1.0f);
    outs->phase = phasor < ins->pulseWidth;

    // Set tempo out
    outs->scaledTempo = (outs->measuredTempo * self->tempoMultiply) / self->tempoDivide;

    self->lastScaledClockPhase = scaledClockPhase;
}
