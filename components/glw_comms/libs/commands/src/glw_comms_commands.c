#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "log.h"
#include "parsing_utils.h"

// implements
#include "glw_comms_commands.h"

static glw_comms_callbacks_t callbacks;

// ====================================================
// ============== COMMANDS ============================

static glw_comms_commands_error_t cmd_led_en(const char *args) {
    if (args[0] == '1' && args[1] == 0)
        callbacks.led_en(true);
    else if (args[0] == '0' && args[1] == 0)
        callbacks.led_en(false);
    else {
        LOGE("Bad arguments. Usage: \"led_en 0(1)\"");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    return CMD_ERR_OK;
}

static glw_comms_commands_error_t cmd_brightness_set(const char *args) {
    if (args[0] == 0) {
        LOGE("Expected an argument after brightness_set command");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    unsigned long brightness;

    if (!argparse_parse_ulong(args, &brightness, &args)) {
        LOGE("Bad argument to brightness_set command. Expected an integer between 0-255");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    if (*args != 0) {
        LOGE("Expected a single argument to brightness_set command");
        return CMD_ERR_BAD_ARGUMENTS;
    } else if (brightness > 255) {
        LOGE("Brightness must be in the range 0-255");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    callbacks.brightness_set(brightness);
    return CMD_ERR_OK;
}

static glw_comms_commands_error_t cmd_shutdown(const char *args) {
    if (args[0] != 0) {
        LOGE("Expected no arguments to shutdown command");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    callbacks.shutdown();
    return CMD_ERR_OK;
}

static glw_comms_commands_error_t cmd_time_set(const char *args) {
    if (args[0] == 0) {
        LOGE("Expected an argument after time_set command");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    unsigned long time_ms;
    if (!argparse_parse_ulong(args, &time_ms, &args)) {
        LOGE("Bad argument to time_set command. Expected an unsigned integer");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    if (*args != 0) {
        LOGE("Expected a single argument to time_set command");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    callbacks.time_set((unsigned long)time_ms);
    return CMD_ERR_OK;
}

static glw_comms_commands_error_t cmd_run(const char *args) {
    if (args[0] == 0) {
        LOGE("Expected a script after run command");
        return CMD_ERR_BAD_ARGUMENTS;
    }

    callbacks.script_run(args);
    return CMD_ERR_OK;
}

// ====================================================

void glw_comms_commands_init(glw_comms_commands_init_t init) {
    assert(init.callbacks.led_en != NULL);
    assert(init.callbacks.brightness_set != NULL);
    assert(init.callbacks.script_run != NULL);
    assert(init.callbacks.shutdown != NULL);
    assert(init.callbacks.time_set != NULL);
    callbacks = init.callbacks;
}

glw_comms_commands_error_t glw_comms_commands_run(const char *input) {
    const char *args;

    if (argparse_check_cmd(input, "led_en", &args))
        return cmd_led_en(args);
    else if (argparse_check_cmd(input, "brightness_set", &args))
        return cmd_brightness_set(args);
    else if (argparse_check_cmd(input, "shutdown", &args))
        return cmd_shutdown(args);
    else if (argparse_check_cmd(input, "time_set", &args))
        return cmd_time_set(args);
    else if (argparse_check_cmd(input, "run", &args))
        return cmd_run(args);
    else {
        LOGE("Unknown command");
        return CMD_ERR_UNKNOWN_COMMAND;
    }
}
