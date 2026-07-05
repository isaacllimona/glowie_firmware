#include <stdbool.h>

#include "native_test_common.h"

#include "glw_commands.h"

FAKE_VOID_FUNC(led_en, bool);
FAKE_VOID_FUNC(brightness_set, uint8_t);
FAKE_VOID_FUNC(shutdown);
FAKE_VOID_FUNC(script_run, const char *);

FAKE_VOID_FUNC(error_cb, char *);

static void assert_no_error() {
    TEST_ASSERT_EQUAL_MESSAGE(0, error_cb_fake.call_count, error_cb_fake.arg0_val);
}

static void assert_error() {
    ASSERT_ONLY_CALL(error_cb);
}

const glw_command_setup_t command_setup = {
    .cmd_callbacks = {
        .led_en = led_en,
        .brightness_set = brightness_set,
        .shutdown = shutdown,
        .script_run = script_run,
    },
    .error_cb = error_cb
};

static void setup_and_run_cmd(char *cmd) {
    glw_command_setup(&command_setup);
    glw_command_run(cmd);
}

void setUp() {
    RESET_FAKE(led_en);
    RESET_FAKE(brightness_set);
    RESET_FAKE(shutdown);
    RESET_FAKE(script_run);

    RESET_FAKE(error_cb);

    FFF_RESET_HISTORY();
}

void tearDown() {}

// ======= TESTS =======
// --- led_en

TEST_CASE(led_enable) {
    setup_and_run_cmd("led_en 1");
    assert_no_error();

    ASSERT_ONLY_CALL(led_en);
    TEST_ASSERT_EQUAL(true, led_en_fake.arg0_val);
}

TEST_CASE(led_disable) {
    setup_and_run_cmd("led_en 0");
    assert_no_error();

    ASSERT_ONLY_CALL(led_en);
    TEST_ASSERT_EQUAL(false, led_en_fake.arg0_val);
}

TEST_CASE(led_enable_no_args) {
    setup_and_run_cmd("led_en");
    assert_error();
}

TEST_CASE(led_enable_wrong_args) {
    setup_and_run_cmd("led_en 2");
    assert_error();
}

// --- brightness_set

TEST_CASE(brightness_set_0) {
    setup_and_run_cmd("brightness_set 0");
    assert_no_error();

    ASSERT_ONLY_CALL(brightness_set);
    TEST_ASSERT_EQUAL(0, brightness_set_fake.arg0_val);
}

TEST_CASE(brightness_set_255) {
    setup_and_run_cmd("brightness_set 255");
    assert_no_error();

    ASSERT_ONLY_CALL(brightness_set);
    TEST_ASSERT_EQUAL(255, brightness_set_fake.arg0_val);
}

TEST_CASE(brightness_set_negative) {
    setup_and_run_cmd("brightness_set -1");
    assert_error();

}

TEST_CASE(brightness_set_too_large) {
    setup_and_run_cmd("brightness_set 256");
    assert_error();
}

// --- shutdown

TEST_CASE(shutdown) {
    setup_and_run_cmd("shutdown");
    assert_no_error();

    ASSERT_ONLY_CALL(shutdown);
}

// --- script_run (via "run" command)

TEST_CASE(run_script) {
    setup_and_run_cmd("run my_script");
    assert_no_error();

    ASSERT_ONLY_CALL(script_run);
    TEST_ASSERT_EQUAL_STRING("my_script", script_run_fake.arg0_val);
}

TEST_CASE(run_script_with_spaces) {
    setup_and_run_cmd("run my long script");
    assert_no_error();

    ASSERT_ONLY_CALL(script_run);
    TEST_ASSERT_EQUAL_STRING("my long script", script_run_fake.arg0_val);
}

TEST_CASE(run_script_no_args) {
    setup_and_run_cmd("run");
    assert_error();
}

// --- invalid command

TEST_CASE(invalid_command) {
    setup_and_run_cmd("invalid_cmd");
    assert_error();
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

    RUN_TEST(test_run_script);
    RUN_TEST(test_run_script_with_spaces);
    RUN_TEST(test_run_script_no_args);

    RUN_TEST(test_invalid_command);

    return UNITY_END();
}
