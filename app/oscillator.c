// oscillator.c
#include "oscillator.h"
#include <math.h>

void initializeBuffer(Buffer *buffer, float frequency) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    buffer->wavetable[i] = sinf(2.0f * M_PI * i / TABLE_SIZE);
  }
  osc->phase = 0.0f;
  osc->phaseIncrement = frequency * TABLE_SIZE / SAMPLE_RATE;
}

float getNextSample(Buffer *buffer) {
  float sample = osc->wavetable[(int)osc->phase];
  osc->phase += osc->phaseIncrement;
  if (osc->phase >= TABLE_SIZE) {
    osc->phase -= TABLE_SIZE;
  }
  return sample;
}
