#include "channel.h"
#include <time.h>

void CH_init(channel_t *self, uint16_t v_max, uint16_t v_cutoff, uint16_t hysteresis, uint16_t random_seed)
{
    thresh_init(&self->_input_thresh, v_cutoff, hysteresis);
    random_init(&self->_random, random_seed);
    gate_init(&self->_gate);
    edge_init(&self->_edge);
    thresh_init(&self->_random_thresh, v_cutoff, 0);
}

void CH_destroy(channel_t *self)
{
    // nothing to do
}

void CH_set_mock_random(channel_t *self, bool doMock)
{
    random_set_mock(&self->_random, doMock);
}

void CH_reset_random(channel_t *self, uint16_t seed)
{
    random_reset(&self->_random, seed);
}

void CH_process(channel_t *self,
    uint16_t *in,
    uint16_t *prob,
    bool *out,
    bool *missed_opportunities)
{
    uint16_t r1, r2;
    // Threshold the input to +/- 2.5V
    uint16_t postThresh;
    thresh_process(&self->_input_thresh, in, &postThresh);

    // // Convert to 0 -> 1 transition
    edge_process(&self->_edge, &postThresh, &r1);

    // // Generate a new random number on an edge
    random_process(&self->_random, &r1, &r2);

    // // Threshold the random value
    thresh_set_cutoff(&self->_random_thresh, 1023 - *prob);
    thresh_process(&self->_random_thresh, &r2, &r1);

    // // Gate the output accordingly
    gate_process(&self->_gate, &postThresh, &r1, &r2);
    *out = r2 > 0;

    // // Gate the Missed Opportunities
    if (missed_opportunities != NULL)
    {
        r2 = !(r1);
        gate_process(&self->_gate, &postThresh, &r2, &r1);
        *missed_opportunities = r1 > 0;
    }
}
