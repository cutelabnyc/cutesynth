#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        double *_vector;
        void *_f(double *);
        uint8_t _numTerms;
    } t_sequence;

    void sequence_init(t_sequence *self, uint8_t numTerms, void (*f)(double *));

    void sequence_process(t_sequence *self, double firstTerm);
    void sequence_setMult(t_sequence *self, void (*f)(double *));

#ifdef __cplusplus
}
#endif

#endif // SEQUENCE_H