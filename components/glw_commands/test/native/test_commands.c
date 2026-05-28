#include <stdbool.h>
#include <stdint.h>

#include <fff.h>
#include <unity.h>

#include "glw_commands.h"

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(led_en, bool);
FAKE_VOID_FUNC(brightness_set, uint8_t);
FAKE_VOID_FUNC(shutdown);
FAKE_VOID_FUNC(script_run, char *);

FAKE_VOID_FUNC(error_cb, char *);

#define ASSERT_NO_CALL(func) TEST_ASSERT_EQUAL(0, func ## _fake.call_count)
#define ASSERT_CALLED(func) TEST_ASSERT_EQUAL(1, func ## _fake.call_count)

void assert_no_error() {
    TEST_ASSERT_EQUAL_MESSAGE(0, error_cb_fake.call_count, error_cb_fake.arg0_val);
}

void assert_error() {
    ASSERT_CALLED(error_cb);

    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(shutdown);
    ASSERT_NO_CALL(script_run);
}

const glw_command_setup_t command_setup = {
    .cmd_callbacks = {
        .led_en = led_en,
        .brightness_set = brightness_set,
        .shutdown = shutdown,
        .script_run = script_run
    },
    .error_cb = error_cb
};

void setup_and_run_cmd(char *cmd) {
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

void test_led_enable(void) {
    setup_and_run_cmd("led_en 1");
    assert_no_error();

    ASSERT_CALLED(led_en);
    TEST_ASSERT_EQUAL(true, led_en_fake.arg0_val);

    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(shutdown);
    ASSERT_NO_CALL(script_run);
}

void test_led_disable(void) {
    setup_and_run_cmd("led_en 0");
    assert_no_error();

    ASSERT_CALLED(led_en);
    TEST_ASSERT_EQUAL(false, led_en_fake.arg0_val);

    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(shutdown);
    ASSERT_NO_CALL(script_run);
}

void test_led_enable_no_args(void) {
    setup_and_run_cmd("led_en");
    assert_error();
}

void test_led_enable_wrong_args(void) {
    setup_and_run_cmd("led_en 2");
    assert_error();
}

// --- brightness_set

void test_brightness_set_0(void) {
    setup_and_run_cmd("brightness_set 0");
    assert_no_error();

    ASSERT_CALLED(brightness_set);
    TEST_ASSERT_EQUAL(0, brightness_set_fake.arg0_val);

    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(shutdown);
    ASSERT_NO_CALL(script_run);
}

void test_brightness_set_255(void) {
    setup_and_run_cmd("brightness_set 255");
    assert_no_error();

    ASSERT_CALLED(brightness_set);
    TEST_ASSERT_EQUAL(255, brightness_set_fake.arg0_val);

    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(shutdown);
    ASSERT_NO_CALL(script_run);
}

void test_brightness_set_too_large(void) {
    setup_and_run_cmd("brightness_set 256");
    assert_error();
}

// --- shutdown

void test_shutdown(void) {
    setup_and_run_cmd("shutdown");
    assert_no_error();

    ASSERT_CALLED(shutdown);
    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(script_run);
}

// --- script_run (via "run" command)

void test_run_script(void) {
    setup_and_run_cmd("run my_script");
    assert_no_error();

    ASSERT_CALLED(script_run);
    TEST_ASSERT_EQUAL_STRING("my_script", script_run_fake.arg0_val);

    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(shutdown);
}

void test_run_script_with_spaces(void) {
    setup_and_run_cmd("run my long script");
    assert_no_error();

    ASSERT_CALLED(script_run);
    TEST_ASSERT_EQUAL_STRING("my long script", script_run_fake.arg0_val);

    ASSERT_NO_CALL(led_en);
    ASSERT_NO_CALL(brightness_set);
    ASSERT_NO_CALL(shutdown);
}

void test_run_script_no_args(void) {
    setup_and_run_cmd("run");
    assert_error();
}

// --- invalid command

void test_invalid_command(void) {
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

    RUN_TEST(test_shutdown);

    RUN_TEST(test_run_script);
    RUN_TEST(test_run_script_with_spaces);
    RUN_TEST(test_run_script_no_args);

    RUN_TEST(test_invalid_command);

    return UNITY_END();
}
