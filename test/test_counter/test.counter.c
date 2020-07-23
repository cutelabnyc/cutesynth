#include <unity.h>
 
#include <cuteop.h>

t_counter c;

void setUp() {
    counter_init(&c, 4);
}

void tearDown() { }
 
void test_no_inc(void)
{
    uint16_t in = 0, out;
    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);
}

void test_inc(void)
{
    uint16_t in = 1, out;
    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 1);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 2);

    in = 0;
    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 3);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 3);
}

void test_wrap(void)
{
    uint16_t in = 1, out;
    counter_init(&c, 2);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 1);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 0);

    counter_process(&c, &in, &out);
    TEST_ASSERT_EQUAL_INT16(out, 1);
}
 
int main(void)
{
  UNITY_BEGIN();
 
  RUN_TEST(test_no_inc);
  RUN_TEST(test_inc);
  RUN_TEST(test_wrap);
 
  return UNITY_END();
}