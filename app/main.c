#include "oscillator.h"
#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>

#define FRAMES_PER_BUFFER 1

// PortAudio callback function
static int audioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  Buffer *buffsky = (Buffer *)userData;
  float *out = (float *)outputBuffer;

  for (unsigned long i = 0; i < framesPerBuffer; i++) {
    *out++ = getNextSample(osc); // Write one sample to the output
  }

  return paContinue;
}

int main() {
  PaError err;
  Oscillator osc;
  initializeOscillator(&osc, 440.0f); // Initialize at A4 (440 Hz)

  // Initialize PortAudio
  err = Pa_Initialize();
  if (err != paNoError) {
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    return 1;
  }

  // Open an audio stream
  PaStream *stream;
  err = Pa_OpenDefaultStream(&stream,
                             0,         // no input channels
                             1,         // mono output
                             paFloat32, // 32-bit floating-point output
                             SAMPLE_RATE, FRAMES_PER_BUFFER,
                             audioCallback, // callback function
                             &osc);         // user data (oscillator)
  if (err != paNoError) {
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    return 1;
  }

  // Start the stream
  err = Pa_StartStream(stream);
  if (err != paNoError) {
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 1;
  }

  printf("Press Enter to stop playback.\n");
  getchar();

  // Stop the stream
  err = Pa_StopStream(stream);
  if (err != paNoError) {
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
  }
  Pa_CloseStream(stream);
  Pa_Terminate();

  return 0;
}
