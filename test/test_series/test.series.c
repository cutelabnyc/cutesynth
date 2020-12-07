#include <cuteop.h>
#include <unity.h>
#include <test_util.h>

#define FIRST_ELEMENT 1
#define NUM_ELEMENTS 10

t_series self;

static void _seriesFunction(double *vector, uint16_t numElements)
{
    vector[0] = FIRST_ELEMENT;
    for (int i = 0; i < numElements; i++)
    {
        if (i != 0)
        {
            vector[i] = vector[i - 1] * 2;
        }
    }
}

void setUp(void)
{
    series_init(&self, NUM_ELEMENTS, _seriesFunction);
}

void tearDown(void) {}

void test_equality_op(void)
{
    uint16_t exp_data[NUM_ELEMENTS] = {
        1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

    series_process(&self, FIRST_ELEMENT);

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        TEST_ASSERT_EQUAL_UINT16(exp_data[i], self._vector[i]);
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_equality_op);
    UNITY_END();

    return 0;
}
