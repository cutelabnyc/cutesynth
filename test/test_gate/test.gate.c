#include <unity.h>
#include <cutesynth.h>
#include <test_util.h>

#define OPEN_SIZE 4
#define CLOSE_SIZE 4

t_gate op;

void setUp()
{
    gate_init(&op);
}

void tearDown() {}

void test_open(void)
{
    uint16_t sig_data[OPEN_SIZE] = {
        1, 2, 3, 4 };
    uint16_t gate_data[OPEN_SIZE] = {
        1, 1, 1, 1 };
    uint16_t out_data[OPEN_SIZE];
    uint16_t exp_data[OPEN_SIZE] = {
        1, 2, 3, 4 };

    for (int i = 0; i < OPEN_SIZE; i++)
    {
        gate_process(&op, sig_data + i, gate_data + i, out_data + i);
        TEST_ASSERT_EQUAL_UINT16(out_data[i], exp_data[i]);
    }
}

void test_closed(void)
{
    uint16_t sig_data[CLOSE_SIZE] = {
        1, 2, 3, 4 };
    uint16_t gate_data[CLOSE_SIZE] = {
        1, 1, 0, 0 };
    uint16_t out_data[CLOSE_SIZE];
    uint16_t exp_data[CLOSE_SIZE] = {
        1, 2, 0, 0 };

    for (int i = 0; i < CLOSE_SIZE; i++)
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
