#include "edge.h"

void edge_init(t_edge *self) { 
    self->_last = 0;
 }

void edge_process(t_edge *self, double *in, double *out)
{
    *out = (self->_last > *in) ? 1 : 0;
//    *out = (self->_last == 0 && *in > 0) ? 1 : 0;
    self->_last = *in;
}
