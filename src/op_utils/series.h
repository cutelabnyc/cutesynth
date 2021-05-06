#ifndef series_H
#define series_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * generator_t: function pointer to represent a mathematical series
     *
     * float* - series array
     * uint16_t - number of elements
     * void * - any additional arguments
     */
    typedef void (*generator_t)(float *, uint16_t, ...);

    typedef struct
    {
        float *_series;
        generator_t _f;
        uint16_t _numElements;
    } t_series;

    void series_init(t_series *self, uint16_t numElements, generator_t f);
    void series_destroy(t_series *self);
    void series_setNumElements(t_series *self, uint16_t numElements);
    void series_setSeries(t_series *self, generator_t f);
    float *series_getSeries(t_series *self);
    void series_process(t_series *self, float firstElement, uint8_t numArgs);

#ifdef __cplusplus
}
#endif

#endif // SERIES_H
