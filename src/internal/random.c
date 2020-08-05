#include "random.h"

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

void ran1s(int32_t *idum, int32_t *iv, int32_t *iy) // this is the random generator variant used in urn + drunk
{
	int32_t j, k;

	// reseed the generator
	if (*idum <= 0 || !(*iy)) {
		if (*idum == 0) 		// rbs -- only reset to 1 if idum is 0
			*idum = 1;
		else if (*idum < 0)
			*idum = -(*idum);
		// else *idum is already positive
		for (j = NTAB + 7; j >= 0; j--) {
			k = (*idum)/IQ;
			*idum = IA * (*idum - k * IQ) - IR * k;
			if (*idum < 0)
				*idum += IM;
			if (j < NTAB)
				iv[j] = *idum;
		}
	}
	*iy = iv[0];
	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0)
		*idum += IM;
	j = (*iy) / NDIV;
	*iy = iv[j];
	iv[j] = *idum;
}

uint16_t _random_next(t_random *self)
{
	ran1s(&self->_state, self->_iv, &self->_iy);
	return self->_state & 1023; // hardcoded 10 bits
}

void random_init(t_random *self, uint16_t seed)
{
	self->_iy = 0;
	for (size_t i = 0; i < NTAB; i++) self->_iv[i] = 0;
	self->_state = -((int32_t) seed);
	self->_hold = _random_next(self);
	self->_mock = false;
	self->_mockstep = true;
}

void random_process(t_random *self, uint16_t *in, uint16_t *out)
{
	if (*in)
	{
		if (!self->_mock)
		{
			self->_hold = _random_next(self);
		}
		else
		{
			self->_mockstep = !self->_mockstep;
			self->_hold = self->_mockstep ? 1023 : 0;
		}
	}
	*out = self->_hold;
}

void random_reset(t_random *self, uint16_t seed)
{
	self->_iy = 0;
	for (size_t i = 0; i < NTAB; i++) self->_iv[i] = 0;
	self->_state = -((int32_t) seed);
	self->_hold = _random_next(self);
}

void random_set_mock(t_random *self, bool doMock)
{
	self->_mock = doMock;
	self->_hold = 0;
	self->_mockstep = true;
}
