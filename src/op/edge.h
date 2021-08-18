#ifndef EDGE_H
#define EDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double _last;
} t_edge;

void edge_init(t_edge *self);
void edge_process(t_edge *self, double *in, double *out);

#ifdef __cplusplus
}
#endif

#endif // EDGE_H
