#include "series.h"
#include <stdio.h>

void series_init(t_series *self, uint16_t numElements, generator_t f)
{
    self->_numElements = numElements;
    self->_series = (float *)calloc(self->_numElements, sizeof(float));
    self->_f = f;
}

void series_destroy(t_series *self)
{
    free(self->_series);
}

void series_setNumElements(t_series *self, uint16_t numElements)
{
    self->_numElements = numElements;
    self->_series = (float *)calloc(self->_numElements, sizeof(float));
}

void series_setSeries(t_series *self, generator_t f)
{
    self->_f = f;
}

float *series_getSeries(t_series *self)
{
    return self->_series;
}


void series_process(t_series *self, float firstElement, uint8_t numArgs)
{
    self->_series[0] = firstElement;
    self->_f(self->_series, self->_numElements);
}
