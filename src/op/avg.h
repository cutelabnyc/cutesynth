#ifndef AVG_H
#define AVG_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        uint8_t buffer_size;
        uint8_t counter;

        float *buffer;
    } t_avg;

    void avg_init(t_avg *self, uint8_t buffer_size);
    float avg_process(t_avg *self, float sample);

#ifdef __cplusplus
}
#endif

#endif // AVG_H
