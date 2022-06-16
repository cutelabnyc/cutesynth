#include "messd.h"

#include <math.h>

#define MAX_TEMPO (1000)
#define MIN_TEMPO (12)

void MS_init(messd_t *self)
{
    phasor_init(&self->internalClock);

    self->beatsPerMeasure = 1;
    self->subdivisionsPerMeasure = 0;
    self->lastRootClockPhase = 1;
    self->lastScaledClockPhase = 1;
    self->lastModulationSwitch = false;
    self->lastModulationSignal = false;
    self->inRoundTripModulation = false;
    self->lastClock = 0;
    self->measuredPeriod = 0.0f;

#ifdef TRACK_INPUT_CLOCK_PERIOD
    self->msSinceLastLeadingEdge = 500.0f; //120 bpm
#endif

    self->beatCounter = 0;
    self->scaledBeatCounter = 0;
    self->tempoMultiply = 1;
    self->tempoDivide = 1;
    self->previousTempoMultiply = 1;
    self->previousTempoDivide = 1;

    self->invertNeedsReset = false;
    self->modulationPending = false;
    self->resetPending = false;
    self->modulateOnEdgeEnabled = true;
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

static void _MS_setModulationPending(messd_t *self, bool pending)
{
    if (pending) {
        self->modulationPending = true;
    } else {
        self->modulationPending = false;
        self->resetPending = false;
    }
}

static void _MS_handleModulation(messd_t *self, messd_ins_t *ins)
{
    // Leading edge on modulation signal
    if (!self->lastModulationSignal && ins->modulationSignal) {
        // Usually this will cause a pending modulation, except in the unique case where
        // we're jumping back up to "modulation high" when we're in round trip mode
        // and already modulated

        if (self->inRoundTripModulation && self->modulationPending) {
            _MS_setModulationPending(self, false);
        } else {
            _MS_setModulationPending(self, true);
        }
    }

    // Lagging edge on modulation signal
    if (self->lastModulationSignal && !ins->modulationSignal) {
        // Does nothing unless we're in round trip mode. In RT mode,
        // this triggers a modulation if we're in a round trip modulation.
        // If not, it cancels the pending modulation
        if (ins->isRoundTrip) {
            if (self->inRoundTripModulation) {
                _MS_setModulationPending(self, true);
            } else if (self->modulationPending) {
                _MS_setModulationPending(self, false);
            }
        }
    }

    // Lagging edge on modulate button
    if (self->modulateOnEdgeEnabled && self->lastModulationSwitch && !ins->modulationSwitch) {
        _MS_setModulationPending(self, !self->modulationPending);
    }

    if (!ins->modulationSwitch) {
        self->modulateOnEdgeEnabled = true;
    }

    // Any reset
    if (ins->reset && !self->resetPending) {
        _MS_setModulationPending(self, true);
        self->resetPending = true;
        self->modulateOnEdgeEnabled = false;
    }

    self->lastModulationSignal = ins->modulationSignal;
    self->lastModulationSwitch = ins->modulationSwitch;
}

static void _MS_handleModulationLatch(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    if (self->modulationPending)
    {
        if (self->resetPending) {
            self->inRoundTripModulation = false;
            self->modulationPending = false;
            self->tempoMultiply = 1;
            self->tempoDivide = 1;
            self->previousTempoMultiply = 1;
            self->previousTempoDivide = 1;
            outs->eom = true;
        } else if (!self->inRoundTripModulation) {
            if (ins->isRoundTrip)
            {
                self->previousTempoMultiply = self->tempoMultiply;
                self->previousTempoDivide = self->tempoDivide;
                self->inRoundTripModulation = true;
            } else {
                self->inRoundTripModulation = false;
            }

            self->tempoMultiply *= ins->subdivisionsPerMeasure;
            self->tempoDivide *= ins->beatsPerMeasure;
            reduceFraction(self->tempoMultiply, self->tempoDivide, &self->tempoMultiply, &self->tempoDivide);

            while (self->tempoMultiply > 1000 && self->tempoDivide >= 2) {
                self->tempoMultiply = self->tempoMultiply >> 1;
                self->tempoDivide = self->tempoDivide >> 1;
            }

            while (self->tempoDivide > 1000 && self->tempoMultiply >= 2) {
                self->tempoMultiply = self->tempoMultiply >> 1;
                self->tempoDivide = self->tempoDivide >> 1;
            }

            float wrappedTempo = self->measuredTempo * ((float) self->tempoMultiply) / ((float) self->tempoDivide);

            if (wrappedTempo > MAX_TEMPO || wrappedTempo < MIN_TEMPO) {
                while (wrappedTempo > MAX_TEMPO) wrappedTempo /= 2.0f;
                while (wrappedTempo < MIN_TEMPO) wrappedTempo *= 2.0f;
                self->tempoDivide = floorf(self->measuredTempo);
                self->tempoMultiply = floorf(wrappedTempo);
                reduceFraction(self->tempoMultiply, self->tempoDivide, &self->tempoMultiply, &self->tempoDivide);
            }

            self->subdivisionsPerMeasure = self->beatsPerMeasure;

            // Set subdivisions equal to beats upon metric modulation
            ins->subdivisionsPerMeasure = self->beatsPerMeasure;
            outs->eom = true;
        } else {
            self->tempoMultiply = self->previousTempoMultiply;
            self->tempoDivide = self->previousTempoDivide;
            outs->eom = true;
            self->inRoundTripModulation = false;
        }
    }

    // Special case--force us to leave a round trip modulation if we're no longer in round trip
    // mode, but we're in a round trip modoulation
    if (self->inRoundTripModulation && !ins->isRoundTrip) {
        self->tempoMultiply = self->previousTempoMultiply;
        self->tempoDivide = self->previousTempoDivide;
        outs->eom = true;
        self->inRoundTripModulation = false;
    }

    _MS_setModulationPending(self, false);
}

static void _MS_handleLatchHelper(messd_t *self, messd_ins_t *ins)
{
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

static void _MS_handleLatchBeats(messd_t *self, messd_ins_t *ins)
{
    // Update beats
    self->beatsPerMeasure = ins->beatsPerMeasure;

    _MS_handleLatchHelper(self, ins);
}

static void _MS_handleLatchDivs(messd_t *self, messd_ins_t *ins)
{
    // Update subdivisions
    self->subdivisionsPerMeasure = ins->subdivisionsPerMeasure;

    _MS_handleLatchHelper(self, ins);
}

void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float rootClockPhase = 0;
    float scaledClockPhase = 0;
    float measurePhase = 0;
    float measurePhaseFloor = 0;
    float measurePhaseCeiling = 0;
    float subdivision = 0;
    float phasor = 0;
    outs->eom = false;

    // -- Handle an input resetBeatCount
    if (ins->resetBeatCount) {
        if (rootClockPhase < 0.5) {
            self->beatCounter = 0;
            self->scaledBeatCounter = 0;
        } else {
            self->beatCounter = self->tempoDivide - 1;
            self->scaledBeatCounter = self->beatsPerMeasure - 1;
        }
    }

    // Handle a leading edge

#ifdef TRACK_INPUT_CLOCK_PERIOD
    if (ins->ext_clock && !self->lastClock) {
        // Force our internal clock into alignment
        float microsOffset = ((float) ins->microsClockOffset / 1000.0f);
        phasor_set_phase(&self->internalClock, 0.0f);
        self->measuredPeriod = self->msSinceLastLeadingEdge == 0.0f ? 500.0f : self->msSinceLastLeadingEdge;
        self->measuredPeriod -= microsOffset;
        self->msSinceLastLeadingEdge = microsOffset;
    } else {
        self->msSinceLastLeadingEdge += ins->delta;
    }
#else
    if (ins->ext_clock && !self->lastClock) {
        phasor_set_phase(&self->internalClock, 0.0f);
    }
#endif

    self->lastClock = ins->ext_clock;

    if (ins->cheatedMeasuredPeriod > 0) {
        self->measuredPeriod = ((float) ins->cheatedMeasuredPeriod) / 1000.0f;
    }

    rootClockPhase = phasor_step(&self->internalClock, ins->delta / self->measuredPeriod);

    // ==== Root clock calculations
    self->measuredTempo = outs->measuredTempo = MS_PER_MINUTE / self->measuredPeriod;

    // Count beats on the clock
    if (self->lastRootClockPhase - rootClockPhase > 0.5) {
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

    // Potentially enter a "modulation pending" state
    _MS_handleModulation(self, ins);

    // Latch to beat events
    if (self->lastScaledClockPhase - scaledClockPhase > 0.5)
    {
        self->scaledBeatCounter = (self->scaledBeatCounter + 1) % self->beatsPerMeasure;

        // Handle changes
        if (!(ins->latchBeatChangesToDownbeat && self->scaledBeatCounter != 0))
        {
            _MS_handleLatchBeats(self, ins);
        }
        if (!(ins->latchDivChangesToDownbeat && self->scaledBeatCounter != 0))
        {
            _MS_handleLatchDivs(self, ins);
        }

        // Then handle modulation changes
        if (!(ins->latchModulationToDownbeat && self->scaledBeatCounter != 0))
        {
            _MS_handleModulationLatch(self, ins, outs);
        }
    }

    if (!ins->invert)
    {
        self->invertNeedsReset = false;
    }

    // Calculate downbeat
    measurePhase = scaledClockPhase + self->scaledBeatCounter;
    measurePhase /= self->beatsPerMeasure;
    outs->downbeat = measurePhase < (ins->pulseWidth / ((float) self->beatsPerMeasure)) ;

    // Calculate subdivisions
    subdivision = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);
    outs->subdivision = subdivision < ins->pulseWidth;

    // Calculate truncation
    if (ins->truncation > 0.0f) {
        float wrapfraction = ins->truncation * self->beatsPerMeasure;
        wrapfraction = floorf(wrapfraction) + 1;
        int truncation = (int) wrapfraction;
        uint8_t truncatedBeatCount = (self->scaledBeatCounter / truncation) * truncation;
        float measurePhaseInTrunc = fmodf(scaledClockPhase + self->scaledBeatCounter, truncation) / self->beatsPerMeasure;
        float measureOffset = ((float) truncatedBeatCount) / self->beatsPerMeasure;

        // Normally the subdivision phase is just the fractional component of the subdivision count.
        // This changes however if we would overlap with the next truncation, or the end of the measure
        float subdivisionProgress = measurePhaseInTrunc * self->subdivisionsPerMeasure;
        float nextSubdivisionProgress = ceil(subdivisionProgress);
        float subdivisionFrac = 1.0 / self->subdivisionsPerMeasure;

        // Next subdivision would go past the end of the measure
        if (
            truncatedBeatCount / truncation == truncation - 1 &&
            nextSubdivisionProgress / self->subdivisionsPerMeasure + measureOffset > 1.0
        ) {
            subdivision = fmodf(subdivisionProgress, 1.0f);
            subdivision /= (1.0 - (measureOffset + floor(subdivisionProgress) / self->subdivisionsPerMeasure));
            subdivision *= subdivisionFrac;
        }

        // Next subdivision would go past the next truncation
        else if (
            nextSubdivisionProgress / self->subdivisionsPerMeasure > ((float) truncation) / self->beatsPerMeasure
        ) {
            subdivision = fmodf(subdivisionProgress, 1.0f);
            subdivision /= ((float) truncation) / self->beatsPerMeasure - floor(subdivisionProgress) / self->subdivisionsPerMeasure;
            subdivision *= subdivisionFrac;
        }

        // "Normal" situation
        else {
            subdivision = fmodf(subdivisionProgress, 1.0f);
        }

        // In terms of phase
        outs->truncate = subdivision < ins->pulseWidth;
    } else {
        outs->truncate = outs->subdivision;
    }

    // Process phased output
    phasor = fmod(scaledClockPhase + ins->phase, 1.0f);
    outs->phase = phasor < ins->pulseWidth;

    // Set tempo out
    outs->scaledTempo = (outs->measuredTempo * self->tempoMultiply) / self->tempoDivide;

    // Set modulate pending output
    // Note the special case here
    outs->modulationPending = self->modulationPending || (!ins->isRoundTrip && self->inRoundTripModulation);
    outs->resetPending = self->resetPending;
    outs->inRoundTripModulation = self->inRoundTripModulation;

    self->lastScaledClockPhase = scaledClockPhase;
}
