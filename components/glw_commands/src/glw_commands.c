#include <assert.h>
#include <stdbool.h>

#include "parsing_utils.h"

// implements
#include "glw_commands.h"

static glw_command_callbacks_t cmd_callbacks;
static error_cb_t error_cb;

static void send_error(char *msg) {
    if(error_cb != NULL)
        error_cb(msg);
}

// ====================================================
// ============== COMMANDS ============================

static void cmd_led_en(char *args) {
    if (args[0] == '1' && args[1] == 0)
        cmd_callbacks.led_en(true);
    else if (args[0] == '0' && args[1] == 0)
        cmd_callbacks.led_en(false);
    else
        send_error("Bad arguments. Usage: \"led_en 0(1)\"");
}

static void cmd_brightness_set(char *args) {
    if (args[0] == 0) {
        send_error("Expected an argument after brightness_set command");
        return;
    }

    long brightness;
    if (argparse_parse_long(args, &brightness, &args)) {
        if (*args != 0)
            send_error("Expected a single argument to brightness_set command");
        else if (brightness > 255 || brightness < 0)
            send_error("Brightness must be in the range 0-255");
        else {
            cmd_callbacks.brightness_set(brightness);
        }
    } else
        send_error("Bad argument to brightness_set command. Expected an integer between 0-255");
}

static void cmd_shutdown(char *args) {
    if (args[0] != 0) {
        send_error("Expected no arguments to shutdown command");
        return;
    }
    cmd_callbacks.shutdown();
}

static void cmd_run(char *args) {
    if (args[0] == 0) {
        send_error("Expected a script after run command");
        return;
    }
    cmd_callbacks.script_run(args);
}

// ====================================================

void glw_command_setup(const glw_command_setup_t *setup) {
    assert(setup->cmd_callbacks.led_en != NULL);
    assert(setup->cmd_callbacks.brightness_set != NULL);
    assert(setup->cmd_callbacks.script_run != NULL);
    assert(setup->cmd_callbacks.shutdown != NULL);
    cmd_callbacks = setup->cmd_callbacks;

    error_cb = setup->error_cb;
}

void glw_command_run(char *input) {
    char *args;

    if (argparse_check_cmd(input, "led_en", &args))
        cmd_led_en(args);
    else if (argparse_check_cmd(input, "brightness_set", &args))
        cmd_brightness_set(args);
    else if (argparse_check_cmd(input, "shutdown", &args))
        cmd_shutdown(args);
    else if (argparse_check_cmd(input, "run", &args))
        cmd_run(args);
    else {
        send_error("Unknown command");
    }
}
