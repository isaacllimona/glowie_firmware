#include <string.h>
#include <unity.h>

#include "glw_os/common.h"
#include "glw_os/time.h"
#include "glw_os/task.h"

#include "config.h"
#include "glw_script_task.h"
#include "glw_script.h"

#define ASSERT_OK(x) TEST_ASSERT_EQUAL(0, (x))
#define ASSERT_NOT_OK(x) TEST_ASSERT_NOT_EQUAL(0, (x))

#define BUFFER_SIZE CONFIG_SCRIPT_INPUT_BUFFER_SIZE
char large_buffer[BUFFER_SIZE];

char *expected_script;
static void fake_script_task_expect_script(void) {
    while(1) {
        char *received_script = wait_for_input(max_timeout);
        TEST_ASSERT_NOT_NULL(received_script);
        TEST_ASSERT_EQUAL_STRING(expected_script, received_script);
        release_input();
    }
}

static void test_send_script(void) {
    char test_script[] = "some script";
    expected_script = test_script;
    script_task_start(fake_script_task_expect_script);
    ASSERT_OK(glw_script_run_string(test_script));
    script_task_cancel();
}

// Send a short script after a longer one
// The first (longer) script is intended to leave the buffer dirty
// This test ensures the second script is written correctly on top of the dirty buffer
static void test_send_two_scripts(void) {
    script_task_start(fake_script_task_expect_script);

    char test_script_1[] = "some longer script dslafasuiuaspdhfioasdf";
    expected_script = test_script_1;
    ASSERT_OK(glw_script_run_string(test_script_1));

    char test_script_2[] = "a shorter one";
    expected_script = test_script_2;
    ASSERT_OK(glw_script_run_string(test_script_2));

    script_task_cancel();
}

static void test_send_multipart_script(void) {
    char test_script_1[] = "first part \\";
    char test_script_2[] = "second part";
    expected_script = "first part second part";
    script_task_start(fake_script_task_expect_script);
    ASSERT_OK(glw_script_run_string(test_script_1));
    ASSERT_OK(glw_script_run_string(test_script_2));
    script_task_cancel();
}

// Send a long multipart script that fills the entire buffer
static void test_send_long_script(void) {
    script_task_start(fake_script_task_expect_script);

    const size_t input_block_size = 1024;
    const size_t sent_blocks = BUFFER_SIZE / 1024;
    const size_t total_script_size = input_block_size * sent_blocks; // includes string termination

    memset(large_buffer, 1, BUFFER_SIZE);
    large_buffer[total_script_size - 1] = 0;
    expected_script = large_buffer;

    char input[input_block_size + 2];
    memset(input, 1, sizeof(input));
    input[input_block_size] = '\\';
    input[input_block_size + 1] = 0;

    for(int i = 0; i < sent_blocks - 1; i++) {
        ASSERT_OK(glw_script_run_string(input));
    }
    input[input_block_size - 1] = 0;
    ASSERT_OK(glw_script_run_string(input));

    // Ensure another script can run after the buffer has been filled by the last one
    // i.e. the buffer has been reset
    char test_script[] = "this should run fineeeee";
    expected_script = test_script;
    ASSERT_OK(glw_script_run_string(test_script));

    script_task_cancel();
}

static void fake_script_task_do_nothing(void) {os_delay(100);}

static void test_send_script_while_taken(void) {
    script_task_start(fake_script_task_do_nothing);
    ASSERT_OK(glw_script_run_string("dummy"));
    ASSERT_NOT_OK(glw_script_run_string("dummy"));
    release_input();
    script_task_cancel();
}

static void test_send_script_overflow(void) {
    script_task_start(fake_script_task_expect_script);

    const size_t input_block_size = 1024;
    const size_t sent_blocks = BUFFER_SIZE / 1024 + 1; // last one should overflow

    char input[input_block_size + 2];
    memset(input, 1, sizeof(input));
    input[input_block_size] = '\\';
    input[input_block_size + 1] = 0;

    for(int i = 0; i < sent_blocks - 1; i++) {
        ASSERT_OK(glw_script_run_string(input));
    }
    input[input_block_size - 1] = 0;
    ASSERT_NOT_OK(glw_script_run_string(input)); // this should overflow

    // Ensure the buffer got reset and we can send scripts again
    expected_script = "this should run just fine";
    ASSERT_OK(glw_script_run_string("this should run just fine"));
}

void glw_script_test_run_all() {
    RUN_TEST(test_send_script);
    RUN_TEST(test_send_two_scripts);
    RUN_TEST(test_send_multipart_script);
    RUN_TEST(test_send_long_script);
    RUN_TEST(test_send_script_while_taken);
    RUN_TEST(test_send_script_overflow);
}
