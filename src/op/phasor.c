#include "phasor.h"

#include <stdlib.h> // malloc()
#include <stdio.h>  // printf()

#include <math.h>

// initialization
void phasor_init(phasor_t *self)
{
    self->rate = 0.02f;
    self->id = 0.0f;
    self->zero_x = 1;
}

static float _lim_f_n1_1(float in)
{
    return (in < -1.0f ? -1.0f : in > 1.0f ? 1.0f : in);
}

void phasor_time(phasor_t *self, float time_)
{
    self->rate = _lim_f_n1_1(time_);
}

void phasor_reset(phasor_t *self)
{
    self->id = 0.0f;
    self->zero_x = 1;
}

void phasor_phase(phasor_t *self, float id)
{
    self->id += id;
}

int16_t phasor_getZeroCrossing(phasor_t *self)
{
    return self->zero_x;
}

float phasor_step(phasor_t *self, float rate)
{
    float odd = self->id;
    self->rate = rate;
    self->id += self->rate;

    if (self->id >= 2.0f)
    {
        self->id -= 2.0f;
        self->zero_x = 1; // ZERO
    }
    else if ((self->id >= 1.0f) && (odd < 1.0f))
    {
        self->zero_x = -1; // PEAK/TROUGH
    }
    else if (self->id < 0.0f)
    {
        self->id += 2.0f;
        self->zero_x = 1; // ZERO
    }
    else
    {
        self->zero_x = 0;
    }

    return self->id;
}
