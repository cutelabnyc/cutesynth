#include <unity.h>
 
#include <cuteop.h>

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
    uint16_t in = 0, out;
    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);

    in = 1;
    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 1);

    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);
}

void test_down(void)
{
    uint16_t in = 1, out;
    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 1);

    in = 0;
    edge_process(&op, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);
}
 
int main(void)
{
  UNITY_BEGIN();
 
  RUN_TEST(test_flat);
  RUN_TEST(test_up);
  RUN_TEST(test_down);
 
  return UNITY_END();
}