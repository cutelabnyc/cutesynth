#include "random.h"

uint16_t _random_next()
{
    return rand() & 1023; // hardcoded 10 bits
}

void random_init(t_random *self)
{
    self->_hold = _random_next();
	self->_mock = false;
	self->_mockstep = true;
}

void random_process(t_random *self, uint16_t *in, uint16_t *out)
{
    if (*in) {
		if (!self->_mock) {
			self->_hold = _random_next();
		} else {
			self->_mockstep = !self->_mockstep;
			self->_hold = self->_mockstep ? 1023 : 0;
		}
    }
    *out = self->_hold;
}

void random_set_mock(t_random *self, bool doMock)
{
	self->_mock = doMock;
	self->_hold = 0;
	self->_mockstep = true;
}
