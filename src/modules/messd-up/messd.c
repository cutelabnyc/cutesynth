#include "messd.h"

#include <math.h>

#define MAX_TEMPO (1000)
#define MIN_TEMPO (12)

void MS_init(messd_t *self)
{
    phasor_init(&self->internalClock);

    self->lastClock = 0;
    self->measuredTempo = 120.0f;
    self->measuredPeriod = 6000.0f / self->measuredTempo;

    self->beatsPerMeasure = 1;
    self->subdivisionsPerMeasure = 0;
    self->tempoMultiply = 1;
    self->tempoDivide = 1;

    self->rootClockPhase = 1;
    self->scaledClockPhase = 1;
    self->rootClockPhaseOffset = 0.0f;

    self->homeTempoMultiply = 1;
    self->homeTempoDivide = 1;
    self->homeSubdivisionsPerMeasure = 2;
    self->homeBeatsPerMeasure = 2;

    self->rootBeatsSinceModulation = 0;
    self->countdown = 0;
    self->memoizedCountdownMax = 0;
    self->memoizedBeatsPerMeasure = 1;
    self->isLatching = false;

    self->rootBeatCounter = 0;
    self->scaledBeatCounter = 0;

    self->lastModulationSwitch = false;
    self->lastModulationSignal = false;
    self->inRoundTripModulation = false;
    self->modulationPending = false;
    self->resetPending = false;
    self->modulateOnEdgeEnabled = true;

#ifdef TRACK_INPUT_CLOCK_PERIOD
    self->msSinceLastLeadingEdge = 500.0f; //120 bpm
#endif
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

static uint16_t leastCommonMultiple(uint16_t x, uint16_t y)
{
    uint16_t tmpx = x;
    uint16_t tmpy = y;
    reduceFraction(x, y, &tmpx, &tmpy);
    return x * y;
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

static void _MS_startCountdownMemoized(messd_t *self, messd_ins_t *ins)
{
    if (self->memoizedBeatsPerMeasure != self->beatsPerMeasure) {
        self->memoizedCountdownMax = self->tempoDivide * self->beatsPerMeasure;
        self->countdown = 0;
        self->memoizedBeatsPerMeasure = self->beatsPerMeasure;
    }
}

static void _MS_handleModulationLatch(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{

    // === Reset-style modulation. Just set everything back the way it was
    //     and move on with your life
    if (self->resetPending) {
        self->inRoundTripModulation = false;
        self->modulationPending = false;
        self->tempoMultiply = 1;
        self->tempoDivide = 1;
        self->homeTempoMultiply = 1;
        self->homeTempoDivide = 1;
        self->homeBeatsPerMeasure = 2;
        self->homeSubdivisionsPerMeasure = 2;
        self->rootClockPhaseOffset = 0.0f;
        self->memoizedBeatsPerMeasure = 0;
        self->countdown = 0;
        self->memoizedCountdownMax = 0;
        outs->eom = true;
        _MS_setModulationPending(self, false);
        return;
    }

    // === Normal modulation.
    if (!self->inRoundTripModulation) {

        // If the input wants a round trip modulation, then store all this
        // stuff so that you can go back later
        if (ins->isRoundTrip)
        {
            self->homeTempoMultiply = self->tempoMultiply;
            self->homeTempoDivide = self->tempoDivide;
            self->homeSubdivisionsPerMeasure = self->subdivisionsPerMeasure;
            self->homeBeatsPerMeasure = self->beatsPerMeasure;
            self->inRoundTripModulation = true;
        } else {
            self->inRoundTripModulation = false;
        }


        self->tempoMultiply *= ins->subdivisionsPerMeasure;
        self->tempoDivide *= ins->beatsPerMeasure;
        reduceFraction(self->tempoMultiply, self->tempoDivide, &self->tempoMultiply, &self->tempoDivide);
        self->subdivisionsPerMeasure = self->beatsPerMeasure;
        self->rootBeatsSinceModulation = 0;
        self->rootBeatCounter %= self->tempoDivide;

        // Set subdivisions equal to beats on metric modulation
        ins->subdivisionsPerMeasure = self->beatsPerMeasure;

        if (self->isLatching) {
            _MS_startCountdownMemoized(self, ins);
        } else {

            // TODO: handle multiple modulations... though maybe it's already working
            float currentBeatsInRootTimeSignature = ((float) self->scaledBeatCounter + self->scaledClockPhase) * self->tempoDivide / self->tempoMultiply;
            currentBeatsInRootTimeSignature = fmod(currentBeatsInRootTimeSignature, self->tempoDivide);
            self->rootClockPhaseOffset = currentBeatsInRootTimeSignature - (self->rootClockPhase + self->rootBeatCounter);
            if (self->rootClockPhaseOffset < 0) self->rootClockPhaseOffset += self->tempoDivide;
        }

        _MS_setModulationPending(self, false);
        outs->eom = true;
    } else {
        self->tempoMultiply = self->homeTempoMultiply;
        self->tempoDivide = self->homeTempoDivide;
        self->subdivisionsPerMeasure = self->homeSubdivisionsPerMeasure;
        self->beatsPerMeasure = self->homeBeatsPerMeasure;
        ins->beatsPerMeasure = self->beatsPerMeasure;
        ins->subdivisionsPerMeasure = self->subdivisionsPerMeasure;

        if (!self->isLatching) {
            // TODO: figure out a way to do this that doesn't involve duplication
            float currentBeatsInRootTimeSignature = ((float) self->scaledBeatCounter + self->scaledClockPhase) * self->tempoDivide / self->tempoMultiply;
            currentBeatsInRootTimeSignature = fmod(currentBeatsInRootTimeSignature, self->tempoDivide);
            self->rootClockPhaseOffset = currentBeatsInRootTimeSignature - (self->rootClockPhase + self->rootBeatCounter);
            if (self->rootClockPhaseOffset < 0) self->rootClockPhaseOffset += self->tempoDivide;
        }

        outs->eom = true;
        self->inRoundTripModulation = false;
        self->memoizedBeatsPerMeasure = 0;
        self->countdown = 0;
        self->memoizedCountdownMax = 0;
        _MS_setModulationPending(self, false);
    }

    // Special case--force us to leave a round trip modulation if we're no longer in round trip
    // mode, but we're in a round trip modoulation
    // TODO: handle this ridiculous case
    // if (self->inRoundTripModulation && !ins->isRoundTrip) {
    //     self->tempoMultiply = self->homeTempoMultiply;
    //     self->tempoDivide = self->homeTempoDivide;
    //     self->subdivisionsPerMeasure = self->homeSubdivisionsPerMeasure;
    //     self->beatsPerMeasure = self->homeBeatsPerMeasure;
    //     ins->beatsPerMeasure = self->beatsPerMeasure;
    //     ins->subdivisionsPerMeasure = self->subdivisionsPerMeasure;
    //     outs->eom = true;
    //     self->inRoundTripModulation = false;
    // }
}

static void _MS_handleLatchBeats(messd_t *self, messd_ins_t *ins)
{
    // Update beats
    self->beatsPerMeasure = ins->beatsPerMeasure;

    // This will only update the countdown if the beatsPerMeasure have changed
    // TODO: handle this
    // _MS_startCountdownMemoized(self, ins);
}

static void _MS_handleLatchDivs(messd_t *self, messd_ins_t *ins)
{
    // Update subdivisions
    self->subdivisionsPerMeasure = ins->subdivisionsPerMeasure;
}

// Process the modulation signal and switch input and either trigger or
// cancel a modulation accordingly.
static inline void _MS_processModulationInput(messd_t *self, messd_ins_t *ins)
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

// On a clock input leading edge, resets the phase of the internal phasor and
// (if internal time measurements are enabled) measures the period between
// leading edges
static inline void _MS_process_handleInputClock(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
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

    self->measuredTempo = outs->measuredTempo = MS_PER_MINUTE / self->measuredPeriod;
}

// Updates the root clock phase and increments the appropriate counters
static inline void _MS_process_updateRootClockPhase(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float nextRootClockPhase = fmodf(phasor_step(&self->internalClock, ins->delta / self->measuredPeriod), 1.0f);

    // Count beats on the clock
    if (self->rootClockPhase - nextRootClockPhase > 0.5) {
        self->rootBeatsSinceModulation++;
        self->rootBeatCounter = (self->rootBeatCounter + 1) % self->tempoDivide;
        if (self->countdown == 0) self->countdown = self->memoizedCountdownMax;
        if (self->countdown != 0) self->countdown--;
    }
    self->rootClockPhase = nextRootClockPhase;
}

// Updates the scaled clock phase and increments the appropriate counters
// Returns true if the scaled clock had a downbeat
static inline bool _MS_process_updateScaledClockPhase(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float offsetRootMeasurePhase = (self->rootClockPhase + self->rootBeatCounter + self->rootClockPhaseOffset);
    if (offsetRootMeasurePhase > self->tempoDivide) offsetRootMeasurePhase -= self->tempoDivide;

    float nextScaledClockPhase = (offsetRootMeasurePhase * self->tempoMultiply) / self->tempoDivide;
    nextScaledClockPhase = fmod(nextScaledClockPhase, 1.0f);

    bool beatEvent = false;
    if ((self->scaledClockPhase - nextScaledClockPhase > 0.5))
    {
        self->scaledBeatCounter = (self->scaledBeatCounter + 1) % self->beatsPerMeasure;
        beatEvent = true;
    }
    self->scaledClockPhase = nextScaledClockPhase;

    return beatEvent;
}

static inline void _MS_process_triggerLatchedChanges(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
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
    bool shouldModulate = self->inRoundTripModulation && self->countdown == 0
        ||
        !self->inRoundTripModulation && self->scaledBeatCounter == 0
        ||
        !self->isLatching;
    if (shouldModulate && self->modulationPending)
    {
        _MS_handleModulationLatch(self, ins, outs);
    }
}

static inline void _MS_process_calculateTruncationOutput(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float subdivision = outs->subdivision;

    // Calculate truncation
    if (ins->truncation > 0.0f) {
        float wrapfraction = ins->truncation * self->beatsPerMeasure;
        wrapfraction = floorf(wrapfraction) + 1;
        int truncation = (int) wrapfraction;
        uint8_t truncatedBeatCount = (self->scaledBeatCounter / truncation) * truncation;
        float measurePhaseInTrunc = fmodf(self->scaledClockPhase + self->scaledBeatCounter, truncation) / self->beatsPerMeasure;
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
}

static inline void _MS_process_calculateOutputs(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    // ==== Output calculation
    outs->beat = self->scaledClockPhase < ins->pulseWidth;

    // Set tempo out
    outs->scaledTempo = (outs->measuredTempo * self->tempoMultiply) / self->tempoDivide;

    // Set modulate pending output
    // Note the special case here
    outs->modulationPending = self->modulationPending || (!ins->isRoundTrip && self->inRoundTripModulation);
    outs->resetPending = self->resetPending;
    outs->inRoundTripModulation = self->inRoundTripModulation;

    outs->countdown = (int) self->countdown * self->tempoMultiply / (self->tempoDivide * self->beatsPerMeasure);
}

void MS_process(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float measurePhase = 0;
    float measurePhaseFloor = 0;
    float measurePhaseCeiling = 0;
    float subdivision = 0;
    float phasor = 0;
    outs->eom = false;

    // TODO: Handle reset if any of round/one-way or free/latch changes
    self->isLatching = ins->latchModulationToDownbeat;

    // Potentially enter a "modulation pending" state
    _MS_processModulationInput(self, ins);
    _MS_process_handleInputClock(self, ins, outs);
    _MS_process_updateRootClockPhase(self, ins, outs);
    bool onScaledClockDownbeat = _MS_process_updateScaledClockPhase(self, ins, outs);

    // -- Handle an input resetBeatCount
    // TODO: Figure out how to handle resets
    // if (ins->resetBeatCount) {
    //     if (self->rootBeatCounter < 0.5) {
    //         // Need to add a little bit of complex logic here, since we're basically saying that
    //         // the last beat event _should_ have been a downbeat. We can't have known that a
    //         // reset event was coming, so we need to handle downbeat latching now, on the reset
    //         latchEvent = self->scaledBeatCounter != 0;
    //         self->beatCounter = 0;
    //         self->scaledBeatCounter = 0;
    //     } else {
    //         self->beatCounter = self->tempoDivide - 1;
    //         self->scaledBeatCounter = self->beatsPerMeasure - 1;
    //     }
    // }

    if (onScaledClockDownbeat) {
        _MS_process_triggerLatchedChanges(self, ins, outs);
    }

    // Calculate downbeat and measure phase
    measurePhase = self->scaledClockPhase + self->scaledBeatCounter;
    measurePhase /= self->beatsPerMeasure;
    outs->downbeat = measurePhase < (ins->pulseWidth / ((float) self->beatsPerMeasure));

    // Calculate subdivisions
    subdivision = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);
    outs->subdivision = subdivision < ins->pulseWidth;

    _MS_process_calculateTruncationOutput(self, ins, outs);

    _MS_process_calculateOutputs(self, ins, outs);
}
