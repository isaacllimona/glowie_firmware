#include <stdbool.h>

#include "native_test_common.h"

#include "glw_comms_commands.h"
#include "unity.h"

FAKE_VOID_FUNC(led_en, bool);
FAKE_VOID_FUNC(brightness_set, uint8_t);
FAKE_VOID_FUNC(shutdown);
FAKE_VOID_FUNC(script_run, const char *);
FAKE_VOID_FUNC(time_set, unsigned int);

const glw_comms_commands_init_t commands_setup = {
    .callbacks = {
        .led_en = led_en,
        .brightness_set = brightness_set,
        .shutdown = shutdown,
        .script_run = script_run,
        .time_set = time_set
    },
};

static glw_comms_commands_error_t setup_and_run_cmd(char *cmd) {
    glw_comms_commands_init(commands_setup);
    return glw_comms_commands_run(cmd);
}

void setUp() {
    RESET_FAKE(led_en);
    RESET_FAKE(brightness_set);
    RESET_FAKE(shutdown);
    RESET_FAKE(script_run);
    RESET_FAKE(time_set);

    FFF_RESET_HISTORY();
}

void tearDown() {}

// ======= TESTS =======
// --- led_en

TEST_CASE(led_enable) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("led_en 1"));

    ASSERT_ONLY_CALL(led_en);
    TEST_ASSERT_EQUAL(true, led_en_fake.arg0_val);
}

TEST_CASE(led_disable) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("led_en 0"));

    ASSERT_ONLY_CALL(led_en);
    TEST_ASSERT_EQUAL(false, led_en_fake.arg0_val);
}

TEST_CASE(led_enable_no_args) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("led_en"));
}

TEST_CASE(led_enable_wrong_args) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("led_en 2"));
}

// --- brightness_set

TEST_CASE(brightness_set_0) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("brightness_set 0"));

    ASSERT_ONLY_CALL(brightness_set);
    TEST_ASSERT_EQUAL(0, brightness_set_fake.arg0_val);
}

TEST_CASE(brightness_set_255) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("brightness_set 255"));

    ASSERT_ONLY_CALL(brightness_set);
    TEST_ASSERT_EQUAL(255, brightness_set_fake.arg0_val);
}

TEST_CASE(brightness_set_negative) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("brightness_set -1"));

}

TEST_CASE(brightness_set_too_large) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("brightness_set 256"));
}

// --- shutdown

TEST_CASE(shutdown) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("shutdown"));

    ASSERT_ONLY_CALL(shutdown);
}

// --- time_set

TEST_CASE(time_set_zero) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("time_set 0"));

    ASSERT_ONLY_CALL(time_set);
    TEST_ASSERT_EQUAL(0, time_set_fake.arg0_val);
}

TEST_CASE(time_set_large_value) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("time_set 4294967295"));

    ASSERT_ONLY_CALL(time_set);
    TEST_ASSERT_EQUAL(4294967295, time_set_fake.arg0_val);
}

TEST_CASE(time_set_no_args) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("time_set"));
}

TEST_CASE(time_set_negative) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("time_set -1"));
}

TEST_CASE(time_set_extra_args) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("time_set 100 extra"));
}

// --- script_run (via "run" command)

TEST_CASE(run_script) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("run my_script"));

    ASSERT_ONLY_CALL(script_run);
    TEST_ASSERT_EQUAL_STRING("my_script", script_run_fake.arg0_val);
}

TEST_CASE(run_script_with_spaces) {
    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("run my long script"));

    ASSERT_ONLY_CALL(script_run);
    TEST_ASSERT_EQUAL_STRING("my long script", script_run_fake.arg0_val);
}

TEST_CASE(run_script_starting_with_space) {
    // Ensure a space at the beginning of a script is kept
    // This is required for multi-part scripts, where the leading space
    // is in the middle of the complete script

    TEST_ASSERT_EQUAL(CMD_ERR_OK, setup_and_run_cmd("run  a script starting with a space"));

    ASSERT_ONLY_CALL(script_run);
    TEST_ASSERT_EQUAL_STRING(" a script starting with a space", script_run_fake.arg0_val);
}

TEST_CASE(run_script_no_args) {
    TEST_ASSERT_EQUAL(CMD_ERR_BAD_ARGUMENTS, setup_and_run_cmd("run"));
}

// --- invalid command

TEST_CASE(invalid_command) {
    TEST_ASSERT_EQUAL(CMD_ERR_UNKNOWN_COMMAND, setup_and_run_cmd("invalid_cmd"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_led_enable);
    RUN_TEST(test_led_disable);
    RUN_TEST(test_led_enable_no_args);
    RUN_TEST(test_led_enable_wrong_args);

    RUN_TEST(test_brightness_set_0);
    RUN_TEST(test_brightness_set_255);
    RUN_TEST(test_brightness_set_too_large);
    RUN_TEST(test_brightness_set_negative);

    RUN_TEST(test_shutdown);

    RUN_TEST(test_time_set_zero);
    RUN_TEST(test_time_set_large_value);
    RUN_TEST(test_time_set_no_args);
    RUN_TEST(test_time_set_negative);
    RUN_TEST(test_time_set_extra_args);

    RUN_TEST(test_run_script);
    RUN_TEST(test_run_script_with_spaces);
    RUN_TEST(test_run_script_starting_with_space);
    RUN_TEST(test_run_script_no_args);

    RUN_TEST(test_invalid_command);

    return UNITY_END();
}
