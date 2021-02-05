#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CUTE_RANDOM_MAX 1024

#define NTAB 32

	typedef struct
	{
		uint16_t _hold;
		bool _mock;
		bool _mockstep;
		int32_t _iy;
		int32_t _iv[NTAB];
		int32_t _state;
	} t_random;

	void random_init(t_random *self, uint16_t seed);
	void random_process(t_random *self, uint16_t *in, uint16_t *out);
	void random_reset(t_random *self, uint16_t seed);

	// When in mock operation, the random sequence will be a fixed sequence of 0, 1, 0, 1
	// Resetting the mock bool also reset the sequence
	void random_set_mock(t_random *self, bool doMock);

#ifdef __cplusplus
}
#endif

#endif // RANDOM_H
