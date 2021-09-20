#include "phase-locked-loop.h"

#include <math.h>

void phase_locked_loop_init(t_phase_locked_loop *self)
{
    phasor_init(&self->_phasor);

	self->_derivative = 0.0000000000953125;
    self->_alpha = 0.00002053125;
    self->_beta = 0.5 * self->_alpha * self->_alpha;
    self->_frequency = 0;
    self->_phase = 0;
    self->_qsig = 0;
    self->_qref = 0;
    self->_lastReference = 0;
    self->_lastSignal = 0;
    self->_lastErrorSignal = 0;
}

void phase_locked_loop_hint(t_phase_locked_loop *self, float hint)
{
    float forcedHint = hint > 0 ? hint : 0.00002053125;
    self->_alpha =  forcedHint * 0.75;
    self->_beta = 0.5 * self->_alpha * self->_alpha;
}

void phase_locked_loop_set_frequency(t_phase_locked_loop *self, double frequency)
{
    self->_frequency = frequency;
}

float phase_locked_loop_process(t_phase_locked_loop *self, int *in1)
{
    float signal = phasor_step(&self->_phasor, self->_frequency);

    // Skip updating everything if in is negative
    if (*in1 < 0) return signal;

    // Derive binary signals
    char ref = *in1 > 0;
    char sig = signal < 0.5;

    char reset = !(self->_qref && self->_qsig);
    self->_qsig = (self->_qsig || (sig && !self->_lastSignal)) && reset; // Trigger signal flip-flop and leading edge of signal
    self->_qref = (self->_qref || (ref && !self->_lastReference)) && reset; // Trigger reference flip-flop on leading edge of reference

	int errorSignal = self->_qsig - self->_qref;
	if (errorSignal != 0) {
		signal = signal;
	} else {
		signal = signal;
	}
    self->_lastReference = ref;
    self->_lastSignal = sig;
    float filtered_ersig = errorSignal + (errorSignal - self->_lastErrorSignal) * self->_derivative;
    self->_lastErrorSignal = filtered_ersig;

    float nextPhase = self->_phasor.id;
    nextPhase = nextPhase - self->_alpha * filtered_ersig;
    if (nextPhase < -1.) {
        nextPhase = -fmod(-nextPhase, 1.0);
    } else if (nextPhase > 1.0) {
        nextPhase = fmod(nextPhase, 1.0);
    }
    phasor_set_phase(&self->_phasor, nextPhase);

	self->_frequency = self->_frequency - filtered_ersig * self->_beta;
	if (self->_frequency < 0) {
		self->_frequency = 0;
	}
    if (self->_frequency < -1.) {
        self->_frequency = -fmod(-self->_frequency, 1.0);
    } else if (self->_frequency > 1.) {
        self->_frequency = fmod(self->_frequency, 1.0);
    }

    return signal;
}
