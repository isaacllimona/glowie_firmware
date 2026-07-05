#include <unity.h>
#include <fff.h>

// prevent uncalled tests
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wunused-function"

DEFINE_FFF_GLOBALS;

#define TEST_CASE(name) static void test_ ## name (void)

//==== Assertion and mock utilities

#define ASSERT_NO_CALL(func) TEST_ASSERT_EQUAL(0, func ## _fake.call_count)
#define ASSERT_CALLED(func) TEST_ASSERT_EQUAL(1, func ## _fake.call_count)

#define ASSERT_ONLY_CALL(func) \
    TEST_ASSERT_EQUAL_MESSAGE(1, fff.call_history_idx, "Expected a single fake call"); \
    TEST_ASSERT_EQUAL_MESSAGE(1, func ## _fake.call_count, "Function " #func " was not called");
