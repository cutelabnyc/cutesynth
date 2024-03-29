#ifndef THRESH_H
#define THRESH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char _lastOutput;
    uint16_t _cutoff;
	uint16_t _hysteresis;
    uint16_t _downThreshold;
    uint16_t _upThreshold;
} t_thresh;

void thresh_init(t_thresh *self, uint16_t cutoff, uint16_t hysteresis);
void thresh_set_cutoff(t_thresh *self, uint16_t cutoff);
void thresh_process(t_thresh *self, uint16_t *in, char *out);

#ifdef __cplusplus
}
#endif

#endif // THRESH_H
