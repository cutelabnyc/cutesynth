#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>
#include "lut.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum waveform {
        SIN,
        SAW,
        SQUARE,
        TRI
    } waveform_t;

    typedef struct osc
    {
        float rate;
        float id;
        int16_t zero_x;
        waveform_t waveform;
    } osc_t;

    // initialization
    void osc_init(osc_t *self);

    // input fns
    void osc_time(osc_t *self, float time);
    void osc_reset(osc_t *self);

    // status
    int16_t osc_get_zero_crossing(osc_t *self);

    // process
    float osc_step(osc_t *self, float fm);

#ifdef __cplusplus
}
#endif

#endif // OSCILLATOR_H
