#ifndef FBSMOOTH_H
#define FBSMOOTH_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        float previousSample;
    } t_fbsmooth;

    void fbsmooth_init(t_fbsmooth *self);
    float fbsmooth_process(t_fbsmooth *self, float smoothFact, float sample);

#ifdef __cplusplus
}
#endif

#endif // FBSMOOTH_H
