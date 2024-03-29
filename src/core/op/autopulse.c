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

// static float fpow2(const float y)
// {
//   union
//   {
//     float f;
//     int32_t i;
//   } c;

//   int32_t integer = (int32_t)y;
//   if(y < 0) integer = integer-1;
//   float frac = y - (float)integer;
//   c.i = (integer+((int32_t)127)) << 23;
//   // Serial.println(c.i);
//   c.f *= 0.33977f*frac*frac + (1.0f-0.33977f)*frac + 1.0f;
//   return c.f;
// }

// Cheat and assume msec_elapsed is 1
void autopulse_process(t_autopulse *self, uint16_t msec_elapsed_in, uint16_t *out)
{
	// Redefine msec_elapsed to be 3
	uint16_t msec_elapsed = 3;
	static float randomMaxFloat = (float) CUTE_RANDOM_MAX;
	uint16_t rin = 1;
	uint16_t rout;
	self->_elapsed += msec_elapsed;
	bool canFlip = self->_elapsed > self->_limit;
	float probability;

	float exponent = ((float) msec_elapsed * self->_fpmsec);

	#if 1
	// probability = 1.0 - 1.0 / fpow2(exponent);
	probability = 0.25 * exponent;
	#else
	probability = 1.0 - 1.0 / powf(2.0, exponent);
	#endif
	
	#if 0
		// Broken and doesn't work
		uint16_t exponent = msec_elapsed * self->_fpmsec;
		probability = 1.0 - 1.0 / ((float) (1 << exponent));
	#endif

	random_process(&self->_random, &rin, &rout);
	float factor = (float) rout / randomMaxFloat;

	bool didflip =  factor < probability;
	if (didflip && canFlip) {
		self->_elapsed = 0;
		self->_state = !(self->_state);
	}

	*out = self->_state ? 1 : 0;
}
