#include <unity.h>

#include <cutesynth.h>

t_autopulse ap;

void setUp() {
    autopulse_init(&ap);
	autopulse_set_pulses_per_second(&ap, 1.0);
}

void tearDown() { }

void test_basic(void)
{
	autopulse_set_pulses_per_second(&ap, 1);

	const int TEST_COUNT = 100;
    uint16_t in = 0, lastout = 0, out;
	int truecount = 0;
	for (int i = 0; i < TEST_COUNT; i++) {
		autopulse_process(&ap, 1000, &out);
		if (lastout == 0 && out > 0)
			truecount += out;
		lastout = out;
	}

	TEST_ASSERT_UINT16_WITHIN(20, TEST_COUNT / 2, truecount);
}

void test_double(void)
{
	autopulse_set_pulses_per_second(&ap, 2);

	const int TEST_COUNT = 100;
    uint16_t in = 0, lastout = 0, out;
	int truecount = 0;
	for (int i = 0; i < TEST_COUNT; i++) {
		autopulse_process(&ap, 500, &out);
		if (lastout == 0 && out > 0)
			truecount += out;
		lastout = out;
	}

	TEST_ASSERT_UINT16_WITHIN(20, TEST_COUNT / 2, truecount);
}

void test_half(void)
{
	autopulse_set_pulses_per_second(&ap, 0.5);

	const int TEST_COUNT = 100;
    uint16_t in = 0, lastout = 0, out;
	int truecount = 0;
	for (int i = 0; i < TEST_COUNT; i++) {
		autopulse_process(&ap, 2000, &out);
		if (lastout == 0 && out > 0)
			truecount += out;
		lastout = out;
	}

	TEST_ASSERT_UINT16_WITHIN(20, TEST_COUNT / 2, truecount);
}

void test_very_small(void)
{
	autopulse_set_pulses_per_second(&ap, 1);

	const int TEST_COUNT = 100000;
    uint16_t in = 0, lastout = 0, out;
	int truecount = 0;
	for (int i = 0; i < TEST_COUNT; i++) {
		autopulse_process(&ap, 1, &out);
		if (lastout == 0 && out > 0)
			truecount += out;
		lastout = out;
	}

	TEST_ASSERT_UINT16_WITHIN(20, 50, truecount);
}

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(test_basic);
  RUN_TEST(test_double);
  RUN_TEST(test_half);
  RUN_TEST(test_very_small);

  return UNITY_END();
}
