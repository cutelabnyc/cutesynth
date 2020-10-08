/*#include "sequence.h"
#include <stdio.h>

void sequence_init(t_sequence *self, uint8_t numTerms, void (*f)(double *))
{
    self->_numTerms = numTerms;
    self->_vector = (double *)malloc(sizeof(double) * self->_numTerms);
    self->(*f) = (*f)(double *);
}

void sequence_process(t_sequence *self, double firstTerm)
{
    if (self->_vector == NULL)
    {
        fprintf(stderr, "Error in cute-o/sequence.c: self->_vector pointing to null memory! Exiting...\n");
        exit(-1);
    }

    self->_vector[0] = firstTerm;
    for (int i = 1; i < self->_numTerms; i++)
    {
        (self->*f)(double *);
    }
}

void sequence_setFunction(t_sequence *self, void (*f)(double *))
{
}*/