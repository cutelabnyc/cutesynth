#include <unity.h>
#include <cutesynth.h>
#include <test_util.h>

t_edge op;

void setUp() {
    edge_init(&op);
}

void tearDown() { }
 
void test_flat(void)
{
    uint16_t in = 0, out;
    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);
}

void test_up(void)
{
    uint16_t in_data[4] = {
        0, 0, 1, 1};
    uint16_t out_data[4];
    uint16_t exp_data[4] = {
        0, 0, 1, 0};

    run_equality_test(&op, (processor_t)edge_process, in_data, out_data, exp_data, 4);
}

void test_down(void)
{
    uint16_t in_data[4] = {
        1, 1, 0, 0};
    uint16_t out_data[4];
    uint16_t exp_data[4] = {
        1, 0, 0, 0};

    run_equality_test(&op, (processor_t)edge_process, in_data, out_data, exp_data, 4);
}
 
int main(void)
{
  UNITY_BEGIN();
 
  RUN_TEST(test_flat);
  RUN_TEST(test_up);
  RUN_TEST(test_down);
 
  return UNITY_END();
}
