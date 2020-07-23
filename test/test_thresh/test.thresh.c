#include <cuteop.h>
#include <unity.h>
#include <test_util.h>

t_thresh self;

void setUp(void)
{
    // set stuff up here
    thresh_init(&self, 511, 3);
}

void tearDown(void) { }

void test_silence_op(void)
{
    uint16_t in_data[4] = {
        0, 0, 0, 0};
    uint16_t out_data[4];
    uint16_t exp_data[4] = {
        0, 0, 0, 0};

    run_equality_test(&self, (processor_t)thresh_process, in_data, out_data, exp_data, 4);
}

void test_one_crossing_op(void)
{
    thresh_init(&self, 511, 0);

    uint16_t in_data[4] = {
        0, 1023, 0, 1023};
    uint16_t out_data[4];
    uint16_t exp_data[4] = {
        0, 1023, 0, 1023};

    run_equality_test(&self, (processor_t)thresh_process, in_data, out_data, exp_data, 4);
}

void test_hysteresis_op(void)
{
    thresh_init(&self, 511, 3);

    uint16_t in_data[8] = {
        0, 512, 600, 510, 600, 400, 600, 512};
    uint16_t out_data[8];
    uint16_t exp_data[8] = {
        0, 512, 600, 510, 600, 0, 600, 512};

    run_equality_test(&self, (processor_t)thresh_process, in_data, out_data, exp_data, 8);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_silence_op);
    RUN_TEST(test_one_crossing_op);
    RUN_TEST(test_hysteresis_op);
    UNITY_END();

    return 0;
}
