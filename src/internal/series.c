#include "series.h"
#include <stdio.h>

void series_init(t_series *self, uint16_t numElements, generator_t f)
{
    self->_numElements = numElements;
    self->_vector = (double *)calloc(self->_numElements, sizeof(double));
    self->_f = f;
}

void series_destroy(t_series *self)
{
    free(self->_vector);
}

void series_setNumElements(t_series *self, uint16_t numElements)
{
    self->_numElements = numElements;
    self->_vector = (double *)calloc(self->_numElements, sizeof(double));
}

void series_setSeries(t_series *self, generator_t f)
{
    self->_f = f;
}

double* series_getSeries(t_series *self)
{
    return self->_vector;
}


void series_process(t_series *self, double firstElement)
{
    self->_vector[0] = firstElement;
    self->_f(self->_vector, self->_numElements);
}
