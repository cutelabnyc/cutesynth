#include <messd.h>
#include <stdlib.h>
#include <math.h>

void MS_init(messd_t *self)
{
	phasor_init(&self->p_clock);
	phasor_init(&self->p_downbeat);
	phasor_init(&self->p_subdivision);

	edge_init(&self->downEdge);
	edge_init(&self->subEdge);

	self->downbeat = 1;
	self->subdivision = 1;
	self->theta = 0.0f;
}

void MS_destroy(messd_t *self)
{
}

void MS_process(messd_t *self,
	uint16_t *clock_in,
	uint16_t *clock_out,
	uint16_t *downbeat_in,
	uint16_t *downbeat_out,
	uint16_t *subdivision_in,
	uint16_t *subdivision_out,
	uint16_t *phase_in,
	uint16_t *phase_out,
	bool metric_modulation)
{
	// Calculate clock based on tempo in
	float tempo = ((float)*clock_in / (float)UINT16_MAX);
	float phasor = 0;
	float dutyCycle = 0.5f;

	// Check for metric modulation
	if (metric_modulation)
	{
		tempo = (tempo * (float)self->subdivision) / (float)self->downbeat;
	}

	phasor = phasor_step(&self->p_clock, tempo);
	*clock_out = (phasor <= 0.5);

	// Check for clock edges to latch downbeat onto
	uint16_t postEdge;
	edge_process(&self->downEdge, clock_out, &postEdge);

	if (*clock_out)
	{
		self->downbeat = ((float)*downbeat_in / 1024.0) * NUM_DIVISION_VALUES;
	}

	// Process downbeat tempo
	*downbeat_out = phasor_step(&self->p_downbeat, tempo / (float)self->downbeat);

	// Check for downbeat edges to latch subdivisions onto
	edge_process(&self->subEdge, downbeat_out, &postEdge);

	if (postEdge)
	{
		self->subdivision = ((float)*subdivision_in / 1024.0) * NUM_DIVISION_VALUES;
	}

	// Process subdivisions
	*subdivision_out = phasor_step(&self->p_subdivision, (tempo * (float)self->subdivision) / (float)self->downbeat);

	// Process phased output
	self->theta = ((float)*phase_in / 1024);
	*phase_out = fmod(phasor + self->theta, 2) > 1;
}
