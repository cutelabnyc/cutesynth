#include "autopulse.h"
#include "math.h"

void autopulse_init(t_autopulse *self)
{
	self->_fps = 0;
	self->_state = false;
}

/**
 * Set the average number of pulses per second.
 */
void autopulse_set_pulses_per_second(t_autopulse *self, double pulses_per_second)
{
	// We're working in flips, and there's two flips per pulse
	self->_fps = 2.0 * pulses_per_second;
}

/**
 * Fetch the next sample, provided the amount of time that has elapsed since the last process.
 */
void autopulse_process(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out)
{
	double exponent = ((double) msec_elapsed * self->_fps / 1000.0);
	printf("%f\n", exponent);

	double probability = 1.0 - 1.0 / pow(2.0, exponent);
	printf("%f\n", probability);

	double factor = (double) random() / (double) RAND_MAX;
	printf("%f\n", factor);

	bool didflip =  factor < probability;
	if (didflip) {
		self->_state = !(self->_state);
	}

	*out = self->_state ? 1 : 0;
}
