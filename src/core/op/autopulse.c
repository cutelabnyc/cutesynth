#include "autopulse.h"
#include "math.h"
#include <stdio.h>

void autopulse_init(t_autopulse *self, uint16_t seed)
{
	self->_fpmsec = 0;
	self->_state = false;
	self->_elapsed = 0;
	self->_limit = 1000;

	random_init(&self->_random, seed);
}

void autopulse_reset(t_autopulse *self, uint16_t seed)
{
	random_reset(&self->_random, seed);
}

/**
 * Set the average number of pulses per second.
 */
void autopulse_set_pulses_per_second(t_autopulse *self, double pulses_per_second)
{
	// We're working in flips, and there's two flips per pulse
	self->_fpmsec = 2.0 * pulses_per_second / 1000.0;
}

void autopulse_set_minimum_interval(t_autopulse *self, uint16_t msec_int)
{
	self->_limit = msec_int;
}

static void autopulse_process_helper(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out, bool fast)
{
	static float randomMaxFloat = (float) CUTE_RANDOM_MAX;
	uint16_t rin = 1;
	uint16_t rout;
	self->_elapsed += msec_elapsed;
	bool canFlip = self->_elapsed > self->_limit;
	float probability;

	if (!fast) {
		float exponent = ((float) msec_elapsed * self->_fpmsec);
		probability = 1.0 - 1.0 / powf(2.0, exponent);
	} else {
		// Very approximate with sharp break points, but also probably fine.
		uint16_t exponent = msec_elapsed * self->_fpmsec;
		probability = 1.0 - 1.0 / ((float) (1 << exponent));
	}

	random_process(&self->_random, &rin, &rout);
	float factor = (float) rout / randomMaxFloat;

	bool didflip =  factor < probability;
	if (didflip && canFlip) {
		self->_elapsed = 0;
		self->_state = !(self->_state);
	}

	*out = self->_state ? 1 : 0;
}

/**
 * Fetch the next sample, provided the amount of time that has elapsed since the last process.
 */
void autopulse_process(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out)
{
	autopulse_process_helper(self, msec_elapsed, out, false);
}

void autopulse_process_fast(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out)
{
	autopulse_process_helper(self, msec_elapsed, out, true);
}
