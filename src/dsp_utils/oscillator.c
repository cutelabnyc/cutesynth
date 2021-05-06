#include "oscillator.h"

#include <stdlib.h> // malloc()
#include <stdio.h>  // printf()

#include <math.h>

// initialization
void osc_init(osc_t *self)
{
    self->rate = 0.02f;
    self->id = 0.0f;
    self->zero_x = 1;
    self->waveform = SIN;
}

float _lim_f_n1_1(float in)
{
    return (in < -1.0f ? -1.0f : in > 1.0f ? 1.0f : in);
}

// input fns
// expect 0-1, but can accept through -1 for TZ effects
void osc_time(osc_t *self, float time_)
{
    // 1.0 = sample_rate
    // 0.0 = stopped
    // -1. = inverse SR
    self->rate = _lim_f_n1_1(time_);
}

void osc_reset(osc_t *self)
{
    self->id = 0.0f;
    self->zero_x = 1;
}

// status
int16_t osc_get_zero_crossing(osc_t *self)
{
    return (self->zero_x);
}

// nb: incrementers run 0-1 w/ zero cross at 0.5
// single-sample
// NOTE: bigger lookup table and no interpolation
float osc_step(osc_t *self, float fm)
{
    float odd = self->id;
    self->id += self->rate + fm;

    // edge & zero-cross detection
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

    // lookup table w/ linear interpolation
    float fbase = (float)LUT_SIZE_HALF * self->id;
    uint16_t base = (uint16_t)fbase;
    float mix = fbase - (float)base;
    float lut = cos_lut[base];
    return (lut + mix * (cos_lut[base + 1] - lut));
}
