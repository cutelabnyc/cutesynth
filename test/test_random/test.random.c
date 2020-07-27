#include <unity.h>
#include <cuteop.h>
#include <test_util.h>
#include <time.h>

#define MARGIN_OF_CERTAINTY (128)
#define RANDOM_SEED time(0)

t_random op;

void setUp()
{
    random_init(&op, RANDOM_SEED);
}

void tearDown() {}

void test_static(void)
{
    uint16_t in_data[MARGIN_OF_CERTAINTY];
    uint16_t out_data[MARGIN_OF_CERTAINTY];

    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++)
    {
        in_data[i] = 0;
        random_process(&op, in_data + i, out_data + i);

        if (i > 0)
        {
            TEST_ASSERT_EQUAL_UINT16(out_data[i - 1], out_data[i]);
        }
    }
}

void test_changing(void)
{
    uint16_t in_data[MARGIN_OF_CERTAINTY];
    uint16_t out_data[MARGIN_OF_CERTAINTY];

    uint16_t did_change = 0;
    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++)
    {
        in_data[i] = 1;
        random_process(&op, in_data + i, out_data + i);

        if (i > 0)
        {
            if (out_data[i] != out_data[i - 1])
            {
                did_change = 1;
            }
        }
    }

    TEST_ASSERT(did_change);
}

void test_reset(void)
{
    uint16_t out_data[MARGIN_OF_CERTAINTY];
    uint16_t test_data[MARGIN_OF_CERTAINTY];

    random_reset(&op, op._seed);

    uint16_t in = 1;
    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++)
    {
        random_process(&op, &in, out_data + i);
    }

    random_reset(&op, op._seed);

    for (int i = 0; i < MARGIN_OF_CERTAINTY; i++)
    {
        uint16_t test_val;
        random_process(&op, &in, &test_val);
        TEST_ASSERT_EQUAL_UINT16(out_data[i], test_val);
    }
}

void test_mock(void)
{
    uint16_t in_data[6] = {
        0, 1, 0, 1, 0, 0};
    uint16_t out_data[6];
    uint16_t exp_data[6] = {
        0, 0, 0, 1023, 1023, 1023};

    random_set_mock(&op, true);
    run_equality_test(&op, (processor_t)random_process, in_data, out_data, exp_data, 5);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_static);
    RUN_TEST(test_changing);
    RUN_TEST(test_mock);
    RUN_TEST(test_reset);

    return UNITY_END();
}
