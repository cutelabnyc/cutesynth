#include "pdivide.h"

void pdivide_init(t_pdivide *self)
{
    self->count = 0;
    self->lastPulse = 0;
}

float pdivide_process(t_pdivide *self, char signal, int divisor)
{
    // Leading or lagging edge, so that we can work with odd divisors
    if ((!signal && self->lastPulse) || (signal && !self->lastPulse)) {
        self->count++;
    }

    if (self->count >= (divisor << 1)) self->count = self->count % (divisor << 1);

    self->lastPulse = signal;

    return (self->count < divisor);
}
