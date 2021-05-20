#include "fb-smooth.h"

void fbsmooth_init(t_fbsmooth *self)
{
    self->previousSample = 0;
}

float fbsmooth_process(t_fbsmooth *self, float smoothFact, float sample)
{
    self->previousSample = ((1.0f - smoothFact) * sample) + (smoothFact * self->previousSample);
    return self->previousSample;
}
