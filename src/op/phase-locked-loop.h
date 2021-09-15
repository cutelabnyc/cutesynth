#ifndef PHASE_LOCKED_LOOP_H
#define PHASE_LOCKED_LOOP_H

#include <stdint.h>
#include "phasor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phase_locked_loop {
    phasor_t _phasor;
    double _proportion;
    double _derivative;
    char _qsig, _qref, _lastReference, _lastSignal;
    int _lastErrorSignal;
    double _phase, _frequency;
} t_phase_locked_loop;

void phase_locked_loop_init(t_phase_locked_loop *self);
float phase_locked_loop_process(t_phase_locked_loop *self, uint16_t *in1);

#ifdef __cplusplus
}
#endif

#endif // PHASE_LOCKED_LOOP_H
