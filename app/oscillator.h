// oscillator.h
#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

#define TABLE_SIZE 512

typedef struct {
  float wavetable[TABLE_SIZE];
  float phase;
  float phaseIncrement;
} Oscillator;

void initializeOscillator(Oscillator *osc, float frequency);
float getNextSample(Oscillator *osc);

#endif
