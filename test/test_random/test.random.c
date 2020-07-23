#include <unity.h>
#include <cuteop.h>
#include <test_util.h>

#define MARGIN_OF_CERTAINTY     (128)

t_random op;

void setUp() {
    random_init(&op);
}

void tearDown() { }

void test_static(void)
{
    uint16_t in_data[MARGIN_OF_CERTAINTY];
    uint16_t out_data[MARGIN_OF_CERTAINTY];

    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++) {
        in_data[i] = 0;
        random_process(&op, in_data + i, out_data + i);

        if (i > 0) {
            TEST_ASSERT_EQUAL_UINT16(out_data[i - 1], out_data[i]);
        }
    }
}

void test_changing(void)
{
    uint16_t in_data[MARGIN_OF_CERTAINTY];
    uint16_t out_data[MARGIN_OF_CERTAINTY];

    uint16_t did_change = 0;
    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++) {
        in_data[i] = 1;
        random_process(&op, in_data + i, out_data + i);

        if (i > 0) {
            if (out_data[i] != out_data[i - 1]) {
                did_change = 1;
            }
        }
    }

    TEST_ASSERT(did_change);
}

void test_mock(void)
{
    uint16_t in_data[4] = {
		1, 1, 1, 1};
    uint16_t out_data[4];
	uint16_t exp_data[4] = {
		0, 1, 0, 1
	};

	random_set_mock(&op, true);
	run_equality_test(&op, (processor_t)random_process, in_data, out_data, exp_data, 4);
}

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(test_static);
  RUN_TEST(test_changing);

  return UNITY_END();
}
