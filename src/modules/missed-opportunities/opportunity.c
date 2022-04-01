#include "opportunity.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define RESET_RANDOM_SEQUENCE(x) srand(x)
#define DENSITY_RANGE 1023
#define DENSITY_THRESHOLD (DENSITY_RANGE * 0.70)

static void _reset_random_sequence(opportunity_t *self, uint16_t random_seed,
                                   bool doResetAutopulse) {
  if (doResetAutopulse)
    autopulse_reset(&self->_autopulse, random_seed);
  for (char i = 0; i < self->num_channels; i++) {
    CH_reset_random(self->channel + i, random_seed + i);
  }
}

void OP_init(opportunity_t *self, channel_t *channels, uint8_t num_channels, uint16_t v_max,
             uint16_t v_cutoff, uint8_t hysteresis, unsigned int random_seed) {
  // Allocates the number of channels
  self->channel = channels;

  // Initialize threshold and edge for reset seed inlet
  thresh_init(&self->_reset_thresh, v_cutoff, hysteresis);
  autopulse_init(&self->_autopulse, random_seed);
  autopulse_set_minimum_interval(&self->_autopulse, 25);

  // Sets all the default values from [/include/globals.h]
  self->num_channels = num_channels;
  self->v_max = v_max;
  self->v_cutoff = v_cutoff;
  self->hysteresis = hysteresis;
  self->random_seed = random_seed;
  self->reset_high = false;

  // Store these values as an optimization
  self->autopulseCutoffLow = (uint16_t) (AUTO_PPS_ROLLOFF_LOW * self->v_max);
  self->autopulseCutoffHigh = (uint16_t) (AUTO_PPS_ROLLOFF_HIGH * self->v_max);

  // Initialize each channel
  for (char i = 0; i < num_channels; i++) {
    CH_init(&self->channel[i], self->v_max, v_cutoff, self->hysteresis,
            self->random_seed);

    // Initialize probability values
    self->probability[i] = 0;
  }

  // Initialize the random sequence by reseting seed
  _reset_random_sequence(self, random_seed, true);
}

void OP_destroy(opportunity_t *self) { }

void OP_set_seed(opportunity_t *self, unsigned int random_seed) {
  self->random_seed = random_seed;
  _reset_random_sequence(self, random_seed, false);
}

void OP_set_mock_random(opportunity_t *self, bool doMock) {
  for (char i = 0; i < self->num_channels; i++)
    CH_set_mock_random(&self->channel[i], doMock);
}

static void _OP_process_reset(opportunity_t *self, bool reset) {
  // // Reset random value sequence if an edge is detected from reset inlet
  // if (postThresh && !self->reset_high)
  if (reset) {
    _reset_random_sequence(self, self->random_seed, !self->reset_high);
  }

  self->reset_high = reset;
}

static void _OP_process_density(opportunity_t *self, uint16_t *density) {
  uint16_t autopulseDensity;

  float base_probability = ((float)*density) / (float)DENSITY_RANGE;
  uint16_t scaled_probability = powf(base_probability, 0.75f) * DENSITY_RANGE;

  for (char i = 0; i < self->num_channels; i++) {
    if (self->channel[i]._edge._last != 1) {
      self->probability[i] = scaled_probability;
    }
  }

  autopulseDensity = *density;

  float scaleFactor = (float)autopulseDensity / (float)self->v_max;
  float autopulseRange;
  float autopulseOffset;
  if (scaleFactor < AUTO_PPS_ROLLOFF_LOW) {
    autopulseRange = (MIN_AUTO_PPS - 0);
    scaleFactor /= AUTO_PPS_ROLLOFF_LOW;
    autopulseOffset = 0;
  } else if (scaleFactor > AUTO_PPS_ROLLOFF_HIGH) {
    autopulseRange = (CRAZY_AUTO_PPS - MAX_AUTO_PPS);
    scaleFactor =
        (scaleFactor - AUTO_PPS_ROLLOFF_HIGH) / (1.0 - AUTO_PPS_ROLLOFF_HIGH);
    autopulseOffset = MAX_AUTO_PPS;
  } else {
    autopulseRange = (MAX_AUTO_PPS - MIN_AUTO_PPS);
    scaleFactor = (scaleFactor - AUTO_PPS_ROLLOFF_LOW) /
                  (1.0 - (AUTO_PPS_ROLLOFF_HIGH - AUTO_PPS_ROLLOFF_LOW));
    autopulseOffset = MIN_AUTO_PPS;
  }
  autopulse_set_pulses_per_second(
      &self->_autopulse, scaleFactor * autopulseRange + autopulseOffset);
}

// Assumes that the maximum value for density is 1023 (2^10 - 1)
static void _OP_process_density_optimized(opportunity_t *self, uint16_t density)
{
  uint16_t autopulseDensity;

  // We want to apply a slight nonlinearity to density here.
  // This is equivalent to multiplying by ~3 near zero
  // ...multiplying by 1.414 at 0.25
  // ...multiplying by 1.18 at 0.5
  // ...multiplying by 1.07 at 0.75
  // ...multiplying by 1 at 1 (aka max density)
  uint16_t scaledProbability;
  uint16_t interpolationFactor = density & (1 << 8 - 1); // least significant 7 bits
  if (density <= 255) {
    // 99 / 70 is a rational approximation of 1.414
    // This multiplies by 2 rather than 3 near zero, which gives a slightly better fitting curve
    scaledProbability = density * (interpolationFactor * 99 / 70 + (255 - interpolationFactor) << 1) >> 8;
  } else if (density <= 511) {
    // 6 / 5 is a rational approximation of 1.18
    scaledProbability = density * (interpolationFactor * 6 / 5 + (255 - interpolationFactor) * 99 / 70) >> 8;
  } else if (density <= 767) {
    // 27 / 25 is an approximation of 1.07
    scaledProbability = density * (interpolationFactor * 27 / 25 + (255 - interpolationFactor) * 6 / 5) >> 8;
  } else {
    scaledProbability = density * (interpolationFactor + (255 - interpolationFactor) * 27 / 25) >> 8;
  }

  // Now scaledProbability should be just slightly larger than the original density, applying a nice nonlinearity
  // Next we need to calculate pulses per second, which depends on scaledProbability as a piecewise linear curve
  // We can use the same approach here to avoid floating point calculations.
  uint16_t pulsesPerSecond, interpolationFactor, interpolationRange;
  if (scaledProbability < self->autopulseCutoffLow) {
    // interpolation is very easy here
    pulsesPerSecond = MIN_AUTO_PPS * scaledProbability / self->autopulseCutoffLow;
  } else if (scaledProbability < self->autopulseCutoffHigh) {
    interpolationFactor = scaledProbability - self->autopulseCutoffLow;
    interpolationRange = self->autopulseCutoffHigh - self->autopulseCutoffLow;
    pulsesPerSecond = (interpolationFactor * MAX_AUTO_PPS + (interpolationRange - interpolationFactor) * MIN_AUTO_PPS) / interpolationRange;
  } else {
    interpolationFactor = scaledProbability - self->autopulseCutoffHigh;
    interpolationRange = self->v_max - self->autopulseCutoffHigh;
    pulsesPerSecond = (interpolationFactor * CRAZY_AUTO_PPS + (interpolationRange - interpolationFactor) * MAX_AUTO_PPS) / interpolationRange;
  }
  autopulse_set_pulses_per_second(
    &self->_autopulse, pulsesPerSecond);
}

static void _OP_process_CV(opportunity_t *self, uint16_t *input,
                           bool *output, bool *missed_opportunities) {
  // Cycles through the channels and processes the CV sent to each channel
  for (int i = 0; i < self->num_channels; i++) {
    // Process the channel array and send the probability gates
    CH_process(&self->channel[i], &input[i], &self->probability[i], &output[i],
               i < self->num_channels - 1 ? &missed_opportunities[i] : NULL);
  }
}

static void OP_process_helper(opportunity_t *self, uint16_t *input, bool *output,
                      bool reset, uint16_t *density, uint16_t *autopulse,
                      bool *missed_opportunities, char msec, unsigned long (*timer)(void), void (*f)(char *), bool debug)
{
  uint16_t tu, tv;
  char buf[32];


  // Process reset input
  if (debug)
    tu = timer();
  _OP_process_reset(self, reset);
  if (debug) {
    tv = timer();
    sprintf(buf, "a: %u\t", tv - tu);
    f(buf);
  }

  // Process density input
  if (debug)
    tu = timer();
  _OP_process_density(self, density);
  if (debug) {
    tv = timer();
    sprintf(buf, "b: %u\t", tv - tu);
    f(buf);
  }

  // Process the automatic pulsing
  if (debug)
    tu = timer();
  autopulse_process(&self->_autopulse, msec, autopulse);
  if (debug) {
    tv = timer();
    sprintf(buf, "c: %u\t", tv - tu);
    f(buf);
  }

  *autopulse = (*autopulse > 0) ? self->v_max : 0;

  // Process CV inputs
  if (debug)
    tu = timer();
  _OP_process_CV(self, input, output, missed_opportunities);
  if (debug) {
    tv = timer();
    sprintf(buf, "d: %u\n", tv - tu);
    f(buf);
  }
}

void OP_process(opportunity_t *self, uint16_t *input, bool *output,
                bool reset, uint16_t *density, uint16_t *autopulse,
                bool *missed_opportunities, char msec)
{
  OP_process_helper(self, input, output, reset, density, autopulse, missed_opportunities, msec, NULL, NULL, false);
}

void OP_process_debug(opportunity_t *self, uint16_t *input, bool *output,
                      bool reset, uint16_t *density, uint16_t *autopulse,
                      bool *missed_opportunities, char msec, unsigned long (*timer)(void), void (*f)(char *))
{
  OP_process_helper(self, input, output, reset, density, autopulse, missed_opportunities, msec, timer, f, true);
}
