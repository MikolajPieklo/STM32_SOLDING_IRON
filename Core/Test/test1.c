#include <unity.h>

void setUp(void)
{
   // set stuff up here
}

void tearDown(void)
{
   // clean stuff up here
}

void test_function_should_doBlahAndBlah(void)
{
   TEST_ASSERT_EQUAL(1,2);
   //test stuff
}

void test_function_should_doAlsoDoBlah(void)
{
   //more test stuff
}

// not needed when using generate_test_runner.rb
int main(void)
{
   UNITY_BEGIN();
   RUN_TEST(test_function_should_doBlahAndBlah);
   RUN_TEST(test_function_should_doAlsoDoBlah);
   return UNITY_END();
}
