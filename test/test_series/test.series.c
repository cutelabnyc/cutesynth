#include <cuteop.h>
#include <unity.h>
#include <test_util.h>

t_series self;

void setUp(void)
{
}

void tearDown(void) {}

void test_equality_op(void)
{
    uint16_t in_data[4] = {
        0, 0, 0, 0};
    uint16_t out_data[4];
    uint16_t exp_data[4] = {
        0, 0, 0, 0};

    //run_equality_test(&self, (processor_t)thresh_process, in_data, out_data, exp_data, 4);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_equality_op);
    UNITY_END();

    return 0;
}
