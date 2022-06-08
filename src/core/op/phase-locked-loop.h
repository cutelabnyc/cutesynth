#ifndef PHASE_LOCKED_LOOP_H
#define PHASE_LOCKED_LOOP_H

#include <stdint.h>
#include "phasor.h"
#include "fdivide.h"
#include "pdivide.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phase_locked_loop {
    phasor_t _phasor;
    double _derivative;
    double _alpha, _beta;
    char _qsig, _qref, _lastReference, _lastSignal;
    int _lastErrorSignal;
    double _phase, _frequency, _lastFrequency;
    int _feedbackDivisor, _referenceDivisor;
    t_fdivide _feedbackDivider;
    t_pdivide _referenceDivider;

} t_phase_locked_loop;

void phase_locked_loop_init(t_phase_locked_loop *self);
void phase_locked_loop_hint(t_phase_locked_loop *self, float hint);

void phase_locked_loop_set_frequency(t_phase_locked_loop *self, double frequency);
void phase_locked_loop_set_numerator(t_phase_locked_loop *self, int numerator);
void phase_locked_loop_set_denominator(t_phase_locked_loop *self, int denominator);

// Set in negative to disable sync
float phase_locked_loop_process(t_phase_locked_loop *self, int *in1);

#ifdef __cplusplus
}
#endif

#endif // PHASE_LOCKED_LOOP_H
