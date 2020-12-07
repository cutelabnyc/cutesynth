#ifndef series_H
#define series_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*generator_t)(double *);

    typedef struct
    {
        double *_vector;
        generator_t _f;
        uint8_t _numElements;
    } t_series;

    void series_init(t_series *self, uint8_t _numElements, void (*f)(double *));

    void series_process(t_series *self, double firstTerm);
    void series_set(t_series *self, void (*f)(double *));

#ifdef __cplusplus
}
#endif

#endif // series_H