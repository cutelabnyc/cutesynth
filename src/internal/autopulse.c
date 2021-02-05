#include "autopulse.h"
#include "math.h"
#include <stdio.h>

void autopulse_init(t_autopulse *self)
{
	self->_fps = 0;
	self->_state = false;
	self->_elapsed = 0;
	self->_limit = 1000;
}

/**
 * Set the average number of pulses per second.
 */
void autopulse_set_pulses_per_second(t_autopulse *self, double pulses_per_second)
{
	// We're working in flips, and there's two flips per pulse
	self->_fps = 2.0 * pulses_per_second;
}

void autopulse_set_minimum_interval(t_autopulse *self, uint16_t msec_int)
{
	self->_limit = msec_int;
}

/**
 * Fetch the next sample, provided the amount of time that has elapsed since the last process.
 */
void autopulse_process(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out)
{
	self->_elapsed += msec_elapsed;
	bool canFlip = self->_elapsed > self->_limit;
	double exponent = ((double) msec_elapsed * self->_fps / 1000.0);

	double probability = 1.0 - 1.0 / pow(2.0, exponent);

	double factor = (double) rand() / (double) RAND_MAX;

	bool didflip =  factor < probability;
	if (didflip && canFlip) {
		self->_elapsed = 0;
		self->_state = !(self->_state);
	}

	*out = self->_state ? 1 : 0;
}
