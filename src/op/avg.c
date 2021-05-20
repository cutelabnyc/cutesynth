#include "avg.h"

void avg_init(t_avg *self, uint8_t buffer_size)
{
    self->buffer_size = buffer_size;
    self->counter = 0;

    self->buffer = (float *)calloc(sizeof(float), self->buffer_size);
}

float avg_process(t_avg *self, float sample)
{
    self->buffer[self->counter] = sample;
    self->counter = (self->counter + 1) % self->buffer_size;

    float avg = 0;
    for (int i = 0; i < self->buffer_size; i++)
    {
        avg += self->buffer[i];
    }
    return avg / self->buffer_size;
}
