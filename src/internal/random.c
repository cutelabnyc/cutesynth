#include "random.h"

uint16_t _random_next()
{
    return rand() & 1023; // hardcoded 10 bits
}

void random_init(t_random *self)
{
    self->_hold = _random_next();
	self->_mock = false;
	self->_mockstep = false;
}

void random_process(t_random *self, uint16_t *in, uint16_t *out)
{
    *out = self->_hold;

    if (*in) {
		if (!self->_mock) {
			self->_hold = _random_next();
		} else {
			self->_hold = self->_mockstep ? 1 : 0;
			self->_mockstep = !self->_mockstep;
		}
    }
}

void random_set_mock(t_random *self, bool doMock)
{
	self->_mock = doMock;
	self->_mockstep = false;
}
