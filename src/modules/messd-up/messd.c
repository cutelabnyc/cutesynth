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
    self->patternFactor = 0.0f;
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
    self->originalBeatCounter = 0;
    self->originalBeatsPerMeasure = 0;

#ifdef USE_TEMPO_NUDGE
    self->nudgeFactor = 1.0;
    self->lastPreNudgedScaledPhase = 0.0;
#endif // USE_TEMPO_NUDGE

    self->lastModulationLatchSetting = -1;
    self->lastRoundTripSetting = -1;

    self->rootBeatCounter = 0;
    self->scaledBeatCounter = 0;

    self->lastModulationSwitch = false;
    self->lastModulationSignal = false;
    self->inRoundTripModulation = false;
    self->modulationPending = false;
    self->modulationForced = false;
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

#ifdef USE_TEMPO_NUDGE
// Returns the factor by which the current scaled clock must be sped up or slowed down to guarantee that
// the two clocks will align on the next downbeat
static void _MS_nudgeScaledClockIntoPhase(messd_t *self)
{
    uint8_t originalBeatsPerMeasure = self->originalBeatsPerMeasure == 0 ? self->beatsPerMeasure : self->originalBeatsPerMeasure;
    float originalMeasurePhase = (self->originalBeatCounter + self->rootClockPhase) / originalBeatsPerMeasure;
    float scaledMeasurePhase = (self->scaledBeatCounter + self->scaledClockPhase) / self->beatsPerMeasure;
    float scaleFactor = (1.0f - scaledMeasurePhase) / (1.0f - originalMeasurePhase);
    scaleFactor *= ((float) self->tempoDivide) / ((float) self->tempoMultiply);
    self->nudgeFactor = scaleFactor;
}
#endif // USE_TEMPO_NUDGE

static void _MS_setModulationPending(messd_t *self, messd_ins_t *ins, bool pending)
{
    if (pending) {
        self->modulationPending = true;

        #ifdef USE_TEMPO_NUDGE
        if (ins->latchModulationToDownbeat && !ins->isRoundTrip) {
            _MS_nudgeScaledClockIntoPhase(self);
        }
        #endif // USE_TEMPO_NUDGE
    } else {
        self->modulationPending = false;
        self->modulationForced = false;
        self->resetPending = false;

        #ifdef USE_TEMPO_NUDGE
        self->nudgeFactor = 1.0;
        #endif // USE_TEMPO_NUDGE
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
        self->originalBeatsPerMeasure = 0;
        self->originalBeatCounter = 0;
        outs->eom = true;
        _MS_setModulationPending(self, ins, false);
        return;
    }

    // If this is the first modulation, store the original time signature
    if (self->originalBeatsPerMeasure == 0) {
        self->originalBeatsPerMeasure = self->beatsPerMeasure;
    }

    // === Normal modulation.
    if (!self->inRoundTripModulation) {

        // If the input wants a round trip modulation, then store all this
        // stuff so that you can go back later
        if (ins->isRoundTrip)
        {
            self->homeTempoMultiply = self->tempoMultiply;
            self->homeTempoDivide = self->tempoDivide;
            self->homeSubdivisionsPerMeasure = ins->subdivisionsPerMeasure;
            self->homeBeatsPerMeasure = ins->beatsPerMeasure;
            self->inRoundTripModulation = true;
        } else {
            self->inRoundTripModulation = false;
        }

        // Reduce the factors before you multiply, requiring fewer divisions
        uint16_t mul = ins->subdivisionsPerMeasure;
        uint16_t div = ins->beatsPerMeasure;
        reduceFraction(mul, div, &mul, &div);
        reduceFraction(mul, self->tempoDivide, &mul, &self->tempoDivide);
        reduceFraction(self->tempoMultiply, div, &self->tempoMultiply, &div);
        self->tempoMultiply *= mul;
        self->tempoDivide *= div;

        // If the factors are both really big, then just scale them down, losing a bit of precision
        while (self->tempoDivide > 1000 && self->tempoMultiply > 1000) {
            self->tempoDivide = self->tempoDivide >> 1;
            self->tempoMultiply = self->tempoMultiply >> 1;
        }

        // If you would bring the tempo over 1000.0, multiply by half as much
        while (self->measuredTempo * self->tempoMultiply / self->tempoDivide > 1000.0) {
            self->tempoMultiply = self->tempoMultiply >> 1;
        }

        // If you would bring the tempo under 1.0, divide by half as much
        while (self->measuredTempo * self->tempoMultiply / self->tempoDivide < 1.0) {
            self->tempoDivide = self->tempoDivide >> 1;
        }

        // Update according to modulation style
        if (ins->modulationStyle == 0) {
            // sync
            self->subdivisionsPerMeasure = self->beatsPerMeasure;
        } else if (ins->modulationStyle == 1) {
            // No-op, this is stay mode
        } else {
            // flip
            int tmp = self->subdivisionsPerMeasure;
            self->subdivisionsPerMeasure = self->beatsPerMeasure;
            self->beatsPerMeasure = tmp;
        }

        self->rootBeatsSinceModulation = 0;
        self->rootBeatCounter %= self->tempoDivide;

        // Update counts on metric modulation
        ins->subdivisionsPerMeasure = self->subdivisionsPerMeasure;
        ins->beatsPerMeasure = self->beatsPerMeasure;

        if (ins->latchModulationToDownbeat && ins->isRoundTrip) {
            _MS_startCountdownMemoized(self, ins);
        } else {
            float currentBeatsInRootTimeSignature = ((float) self->scaledBeatCounter + self->scaledClockPhase) * self->tempoDivide / self->tempoMultiply;
            currentBeatsInRootTimeSignature = fmod(currentBeatsInRootTimeSignature, self->tempoDivide);
            self->rootClockPhaseOffset = currentBeatsInRootTimeSignature - (self->rootClockPhase + self->rootBeatCounter);
            if (self->rootClockPhaseOffset < 0) self->rootClockPhaseOffset += self->tempoDivide;
        }

        _MS_setModulationPending(self, ins, false);
        outs->eom = true;
    } else {
        self->tempoMultiply = self->homeTempoMultiply;
        self->tempoDivide = self->homeTempoDivide;
        self->rootBeatCounter %= self->tempoDivide;
        self->subdivisionsPerMeasure = self->homeSubdivisionsPerMeasure;
        self->beatsPerMeasure = self->homeBeatsPerMeasure;
        ins->beatsPerMeasure = self->beatsPerMeasure;
        ins->subdivisionsPerMeasure = self->subdivisionsPerMeasure;

        // In round-trip/free mode, there's no need to do this phase computation, because
        // you're already guaranteed to be in phase by virtue of the countdown.
        if (!ins->latchModulationToDownbeat) {
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
        self->originalBeatsPerMeasure = 0;
        _MS_setModulationPending(self, ins, false);
    }
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
static inline void _MS_processModulationInput(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    bool modulationWouldHaveNoEffect = (!self->inRoundTripModulation) && (ins->beatsPerMeasure == ins->subdivisionsPerMeasure);

    // Leading edge on modulation signal
    if (!self->lastModulationSignal && ins->modulationSignal) {

        // Leading edge on the modulation signal causes us to enter a pending modulation,
        // unless we're already in a pending modulation
        if (!self->modulationPending && !modulationWouldHaveNoEffect) {
            _MS_setModulationPending(self, ins, true);
        } else if (modulationWouldHaveNoEffect) {
            outs->modulationRequestSkipped = true;
        }
    }

    // Lagging edge on modulate button
    if (self->modulateOnEdgeEnabled && !self->lastModulationSwitch && ins->modulationSwitch) {
        if (!self->modulationPending && !modulationWouldHaveNoEffect) {
            _MS_setModulationPending(self, ins, !self->modulationPending);
        } else if (modulationWouldHaveNoEffect) {
            outs->modulationRequestSkipped = true;
        } else  {
            self->modulationForced = true;
        }
    }

    if (!ins->modulationSwitch) {
        self->modulateOnEdgeEnabled = true;
    }

    bool forceReset = (ins->isRoundTrip != self->lastRoundTripSetting && self->lastRoundTripSetting != -1);
    forceReset |= (ins->latchModulationToDownbeat != self->lastModulationLatchSetting && self->lastModulationLatchSetting != -1);
    self->lastRoundTripSetting = ins->isRoundTrip;
    self->lastModulationLatchSetting = ins->latchModulationToDownbeat;

    // Any reset
    if ((ins->reset || forceReset) && !self->resetPending) {
        _MS_setModulationPending(self, ins, true);
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
        if (self->tempoDivide != 1 || self->tempoMultiply != 1) {
            self->originalBeatCounter = (self->originalBeatCounter + 1) % self->originalBeatsPerMeasure;
        }
    }
    self->rootClockPhase = nextRootClockPhase;
}

// Updates the scaled clock phase and increments the appropriate counters
// Returns true if the scaled clock had a downbeat
static inline bool _MS_process_updateScaledClockPhase(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float offsetRootMeasurePhase = (self->rootClockPhase + self->rootBeatCounter + self->rootClockPhaseOffset);
    if (offsetRootMeasurePhase > self->tempoDivide) offsetRootMeasurePhase -= self->tempoDivide;

    float nextRawScaledClockPhase = (offsetRootMeasurePhase * self->tempoMultiply) / self->tempoDivide;
    float nextScaledClockPhase;

    // Okay, here's where things get interesting. Rather than simply setting the next phase,
    // we compute a delta which we can scale by the nudge factor if we so choose.
#ifdef USE_TEMPO_NUDGE
    if (self->nudgeFactor != 1.0f) {
        float scaledPhaseDelta = nextRawScaledClockPhase - self->lastPreNudgedScaledPhase;
        if (scaledPhaseDelta < 0) scaledPhaseDelta = (float) self->tempoMultiply + scaledPhaseDelta;
        scaledPhaseDelta *= self->nudgeFactor;
        nextScaledClockPhase = self->scaledClockPhase + scaledPhaseDelta;
        nextScaledClockPhase = fmod(nextScaledClockPhase, 1.0f);
    } else {
        nextScaledClockPhase = fmod(nextRawScaledClockPhase, 1.0f);
    }

    self->lastPreNudgedScaledPhase = nextRawScaledClockPhase;
#else
    nextScaledClockPhase = fmod(nextRawScaledClockPhase, 1.0f);
#endif // USE_TEMPO_NUDGE

    bool beatEvent = false;
    if ((self->scaledClockPhase - nextScaledClockPhase > 0.5))
    {
        self->scaledBeatCounter = (self->scaledBeatCounter + 1) % self->beatsPerMeasure;
        beatEvent = true;
    }

    self->scaledClockPhase = nextScaledClockPhase;

    if (self->tempoDivide == 1 && self->tempoMultiply == 1) {
        self->originalBeatCounter = self->scaledBeatCounter;
    }

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
        self->inRoundTripModulation && !ins->resyncToExternal && self->scaledBeatCounter == 0
        ||
        !self->inRoundTripModulation && self->scaledBeatCounter == 0
        ||
        !ins->latchModulationToDownbeat;
    if (ins->modulationSwitch) shouldModulate = false;
    shouldModulate |= self->modulationForced
        ||
        self->resetPending;
    if (shouldModulate && self->modulationPending)
    {
        _MS_handleModulationLatch(self, ins, outs);
    }
}

static inline void _MS_process_calcTenMillisecondPhase(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    float rootTenMillisPhase = 10.0f / self->measuredPeriod; // phase for 10 milliseconds in input clock time
    float scaledTenMillisPhase = rootTenMillisPhase * self->tempoDivide / self->tempoMultiply;
    float subdivTenMillisPhase = scaledTenMillisPhase * self->subdivisionsPerMeasure / self->beatsPerMeasure;

    self->beatPhaseTenMillis = scaledTenMillisPhase;
    self->divPhaseTenMillis = subdivTenMillisPhase;
}

static inline void _MS_process_calculateTruncationOutput(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    // Newest and simplest idea: truncation output is simply the subdivision output, but with
    // a hard reset point somewhere in the measure.

    // For now, don't bother to latch this
    self->patternFactor = ins->truncation;

    // If truncation is less than 0, simply return subdivision
    if (self->patternFactor < 0 || self->patternFactor == 0.0f || self->patternFactor == 1.0f) {
        outs->truncate = outs->subdivision;
        return;
    }

    float truncfac = fmod(self->patternFactor, 1.0f) * self->beatsPerMeasure;
    float measurePhase = self->scaledClockPhase + self->scaledBeatCounter;
    if (measurePhase > truncfac) measurePhase -= truncfac;
    measurePhase /= (float) self->beatsPerMeasure;
    float patternPhase = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);

    outs->truncate = patternPhase < (ins->useTenMillisecondWidth ? self->divPhaseTenMillis : ins->pulseWidth);
}

static inline void _MS_process_calculateOutputs(messd_t *self, messd_ins_t *ins, messd_outs_t *outs)
{
    // ==== Output calculation
    outs->beat = self->scaledClockPhase < (ins->useTenMillisecondWidth ? self->beatPhaseTenMillis : ins->pulseWidth);

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
    outs->modulationRequestSkipped = false;

    // Potentially enter a "modulation pending" state
    _MS_processModulationInput(self, ins, outs);
    _MS_process_handleInputClock(self, ins, outs);
    _MS_process_updateRootClockPhase(self, ins, outs);
    bool onScaledClockDownbeat = _MS_process_updateScaledClockPhase(self, ins, outs);

    // -- Handle an input resetBeatCount
    if (ins->resetBeatCount) {
        self->rootClockPhase = 0.0f;
        self->scaledClockPhase = 0.0f;
        self->rootClockPhaseOffset = 0.0f;
        self->rootBeatCounter = 0;
        self->scaledBeatCounter = 0;
        onScaledClockDownbeat = true;
    }

    if (onScaledClockDownbeat) {
        _MS_process_triggerLatchedChanges(self, ins, outs);
    }

    _MS_process_calcTenMillisecondPhase(self, ins, outs);

    // Calculate downbeat and measure phase
    measurePhase = self->scaledClockPhase + self->scaledBeatCounter;
    measurePhase /= self->beatsPerMeasure;
    float beatPulseWidth = (ins->useTenMillisecondWidth ? self->beatPhaseTenMillis : ins->pulseWidth);
    outs->downbeat = measurePhase < (beatPulseWidth / ((float) self->beatsPerMeasure));

    // Calculate subdivisions
    subdivision = fmod(measurePhase * self->subdivisionsPerMeasure, 1.0f);
    outs->subdivision = subdivision < (ins->useTenMillisecondWidth ? self->divPhaseTenMillis : ins->pulseWidth);

    _MS_process_calculateTruncationOutput(self, ins, outs);

    _MS_process_calculateOutputs(self, ins, outs);
}
