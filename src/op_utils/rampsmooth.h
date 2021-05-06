#ifndef RAMPSMOOTH_H
#define RAMPSMOOTH_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // TODO: How do you pass a data type as a function?
    typedef float t_sample;

    typedef struct {
        uint8_t buffer_size;
        uint8_t counter;

        t_sample *buffer;
    } t_rampsmooth;

    void rampsmooth_init(t_rampsmooth *self, uint8_t buffer_size);
    void rampsmooth_appendSample(t_rampsmooth *self, t_sample sample);
    t_sample rampsmooth_process(t_rampsmooth *self);

#ifdef __cplusplus
}
#endif

#endif // RAMPSMOOTH_H
