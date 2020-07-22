#include <unity.h>
 
#include <cuteop.h>

void setUp() { }
void tearDown() { }
 
void test_the_tests(void)
{
    // No-op
}
 
int main(void)
{
  UNITY_BEGIN();
 
  RUN_TEST(test_the_tests);
 
  return UNITY_END();
}