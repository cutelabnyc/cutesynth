#ifndef PHASOR_H
#define PHASOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct phasor
    {
        float rate;
        float id;
        int16_t zero_x;
    } phasor_t;

    void phasor_init(phasor_t *self);
    void phasor_time(phasor_t *self, float time);
    void phasor_reset(phasor_t *self);

    float phasor_step(phasor_t *self, float rate);

#ifdef __cplusplus
}
#endif

#endif // PHASOR_H
