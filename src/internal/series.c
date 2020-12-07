#include "series.h"
#include <stdio.h>

void series_init(t_series *self, uint8_t numElements, generator_t f)
{
    self->_numElements = numElements;
    self->_vector = (double *)malloc(sizeof(double) * self->_numElements);
    self->_f = f;
}

void series_free(t_series *self)
{
    free(self->_vector);
}

void series_setNumElements(t_series *self, uint8_t numElements)
{
    series_free(self);
    self->_numElements = numElements;
    self->_vector = (double *)malloc(sizeof(double) * self->_numElements);
}

void series_setGenerator(t_series *self, generator_t f)
{
    self->_f = f;
}

void series_process(t_series *self, double firstElement)
{
    if (self->_vector == NULL)
    {
        fprintf(stderr, "Error in cute-o/series.c: self->_vector pointing to null memory! Exiting...\n");
        exit(-1);
    }
    self->_vector[0] = firstElement;
    self->_f(self->_vector, self->_numElements);
}
