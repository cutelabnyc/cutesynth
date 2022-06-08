#include "fdivide.h"

void fdivide_init(t_fdivide *self)
{
    self->lastPhase = 0.0f;
    self->count = 0;
}

float fdivide_process(t_fdivide *self, float signal, int divisor)
{
    if (self->lastPhase > 0.75 && signal < 0.25) {
        self->count++;
    }

    if (self->count >= divisor) self->count = self->count % divisor;

    self->lastPhase = signal;

    return (signal + self->count) / ((float) (divisor <= 0 ? 1 : divisor));
}
