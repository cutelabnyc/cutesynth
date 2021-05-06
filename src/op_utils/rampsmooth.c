#include "rampsmooth.h"

void rampsmooth_init(t_rampsmooth *self, uint8_t buffer_size)
{
    self->buffer_size = buffer_size;
    self->counter = 0;

    self->buffer = (t_sample *)calloc(sizeof(t_sample), self->buffer_size);
}

void rampsmooth_appendSample(t_rampsmooth *self, t_sample sample)
{
    // if (self->counter > self->buffer_size - 1){
    //     Error_Handler();
    // }

    self->buffer[self->counter] = sample;
    self->counter++;
}


t_sample rampsmooth_process(t_rampsmooth *self)
{
    t_sample avg = 0;
    for (int i = 0; i < self->buffer_size; i++)
    {
        avg += self->buffer[i];
    }
    self->counter = 0;
    return avg / self->buffer_size;
}
