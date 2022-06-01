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

    uint16_t r1, r2, r3;
    // Threshold the input to +/- 2.5V
    char postThresh, t1;
    thresh_process(&self->_input_thresh, in, &postThresh);

    // // Convert to 0 -> 1 transition
    r2 = postThresh > 0 ? 1: 0;
    edge_process(&self->_edge, &r2, &r1);

    // // Generate a new random number on an edge
    random_process(&self->_random, &r1, &r2);

    // // Threshold the random value
    int cutoff = 1023 - *prob;
    cutoff = cutoff > 1023 ? 1023 : (cutoff < 0 ? 0 : cutoff);
    thresh_set_cutoff(&self->_random_thresh, cutoff);
    thresh_process(&self->_random_thresh, &r2, &t1);


    // // Gate the output accordingly
    r1 = t1;
    r3 = postThresh;
    gate_process(&self->_gate, &r3, &r1, &r2);
    *out = r2 > 0;

    // // Gate the Missed Opportunities
    if (missed_opportunities != NULL)
    {
        r2 = !(r1);
        gate_process(&self->_gate, &r3, &r2, &r1);
        *missed_opportunities = r1 > 0;
    }
}
