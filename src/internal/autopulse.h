#ifndef AUTOPULSE_H
#define AUTOPULSE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bool _state;
	double _fps;
} t_autopulse;

/**
 * Autopulse automatically pulses between high and low states (1 and 0 output) with some probability.
 */
void autopulse_init(t_autopulse *self);

/**
 * Set the average number of pulses per second.
 */
void autopulse_set_pulses_per_second(t_autopulse *self, double pulses_per_second);

/**
 * Fetch the next sample, provided the amount of time that has elapsed since the last process.
 */
void autopulse_process(t_autopulse *self, uint16_t msec_elapsed, uint16_t *out);

#ifdef __cplusplus
}
#endif

#endif // AUTOPULSE_H
