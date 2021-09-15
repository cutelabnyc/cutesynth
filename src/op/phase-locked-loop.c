#include "phase-locked-loop.h"

void phase_locked_loop_init(t_phase_locked_loop *self)
{
    phasor_init(&self->_phasor);

    self->_derivative = 64;
    self->_proportion = 1. / 128.;
    self->_frequency = 0;
    self->_phase = 0;
    self->_qsig = 0;
    self->_qref = 0;
    self->_lastReference = 0;
    self->_lastSignal = 0;
    self->_lastErrorSignal = 0;
}

float phase_locked_loop_process(t_phase_locked_loop *self, uint16_t *in1)
{
    // TODO: figure out like timestep and all that bullshit...
    float signal = phasor_step(&self->_phasor, self->_frequency);

    // Nonzero for in is "on"
    char ref = *in1 > 0;
    char sig = signal < 0.5;
    char reset = !(self->_qref && self->_qsig);
    self->_qsig = (self->_qsig || (sig && !self->_lastSignal)) && reset; // Trigger signal flip-flop and leading edge of signal
    self->_qref = (self->_qref || (ref && !self->_lastReference)) && reset; // Trigger reference flip-flop on leading edge of reference
    self->_lastReference = ref;
    self->_lastSignal = sig;
    int errorSignal = self->_qref - self->_qsig;
    float filtered_ersig = errorSignal + (errorSignal - self->_lastErrorSignal) * self->_derivative;
    self->_lastErrorSignal = errorSignal;
    self->_frequency = self->_frequency - filtered_ersig * self->_proportion;

    return signal;
}
