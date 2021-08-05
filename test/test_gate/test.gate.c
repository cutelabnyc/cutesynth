#include <unity.h>
#include <cutesynth.h>
#include <test_util.h>

t_gate op;

void setUp()
{
    gate_init(&op);
}

void tearDown() {}

void test_open(void)
{
    const int testSize = 4;

    uint16_t sig_data[testSize] = {
        1, 2, 3, 4};
    uint16_t gate_data[testSize] = {
        1, 1, 1, 1};
    uint16_t out_data[testSize];
    uint16_t exp_data[testSize] = {
        1, 2, 3, 4};

    for (int i = 0; i < testSize; i++)
    {
        gate_process(&op, sig_data + i, gate_data + i, out_data + i);
        TEST_ASSERT_EQUAL_UINT16(out_data[i], exp_data[i]);
    }
}

void test_closed(void)
{
    const int testSize = 4;

    uint16_t sig_data[testSize] = {
        1, 2, 3, 4};
    uint16_t gate_data[testSize] = {
        1, 1, 0, 0};
    uint16_t out_data[testSize];
    uint16_t exp_data[testSize] = {
        1, 2, 0, 0};

    for (int i = 0; i < testSize; i++)
    {
        gate_process(&op, sig_data + i, gate_data + i, out_data + i);
        TEST_ASSERT_EQUAL_UINT16(out_data[i], exp_data[i]);
    }
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_closed);
    RUN_TEST(test_open);

    return UNITY_END();
}
