#ifndef series_H
#define series_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (*generator_t)(double *, uint16_t);

    typedef struct
    {
        double *_vector;
        generator_t _f;
        uint16_t _numElements;
    } t_series;

    void series_init(t_series *self, uint16_t numElements, generator_t f);
    void series_destroy(t_series *self);
    void series_setNumElements(t_series *self, uint16_t numElements);
    void series_setSeries(t_series *self, generator_t f);
    double* series_getSeries(t_series *self);
    void series_process(t_series *self, double firstElement);

#ifdef __cplusplus
}
#endif

#endif // series_H
