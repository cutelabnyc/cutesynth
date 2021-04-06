#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

#define LUT_SIN_SIZE ((uint32_t)1024)
#define LUT_SIN_HALF (LUT_SIN_SIZE >> 1)

#ifdef __cplusplus
extern "C"
{
#endif

    extern const float sine_lut[];

    typedef struct osc
    {
        float rate;
        float id;
        int8_t zero_x;
    } osc_t;

    // initialization
    void osc_init(osc_t* self);

    // input fns
    void osc_time(osc_t *self, float time);
    void osc_reset(osc_t *self);

    // status
    int8_t osc_get_zero_crossing(osc_t *self);

    // process
    float osc_step(osc_t *self, float fm);

#ifdef __cplusplus
}
#endif

#endif // OSCILLATOR_H
