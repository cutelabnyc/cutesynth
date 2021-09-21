#include "oscillator.h"

// initialization
void osc_init(osc_t *self, waveform_t waveform)
{
    self->rate = 0.02f;
    self->id = 0.0f;
    self->zero_x = 1;
    self->waveform = waveform;
}

static float _lim_f_n1_1(float in)
{
    return (in < -1.0f ? -1.0f : in > 1.0f ? 1.0f : in);
}

void osc_time(osc_t *self, float time_)
{
    self->rate = _lim_f_n1_1(time_);
}

void osc_reset(osc_t *self)
{
    self->id = 0.0f;
    self->zero_x = 1;
}

void osc_setWaveform(osc_t *self, waveform_t waveform)
{
    self->waveform = waveform;
}

static float _osc_getSample(osc_t *self, uint16_t base)
{
    switch (self->waveform){
    case TRI:
        return tri_lut_uint16[base];
        break;
    case SAW:
        return saw_lut_uint16[base];
        break;
    case SQUARE:
        return square_lut_uint16[base];
        break;
    case SIN:
    default:
        return sin_lut_uint16[base];
        break;
    }
}

int16_t osc_get_zero_crossing(osc_t *self)
{
    return (self->zero_x);
}


float osc_step(osc_t *self, float fm)
{
    float odd = self->id;
    self->id += self->rate + fm;

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

    // Lookup table w/ linear interpolation
    float fbase = (float)LUT_SIZE_HALF * self->id;
    uint16_t base = (uint16_t)fbase;
    float mix = fbase - (float)base;
    float lut = _osc_getSample(self, base);
    return (lut + mix * (_osc_getSample(self, base + 1) - lut));
}
