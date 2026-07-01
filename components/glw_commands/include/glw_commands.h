#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void (*led_en)(bool enabled);
    void (*brightness_set)(uint8_t value);
    void (*shutdown)(void);
    void (*script_run)(const char *script);
    void (*time_set)(unsigned long ms);
} glw_command_callbacks_t;

typedef void (*error_cb_t)(char *msg);

typedef struct {
    glw_command_callbacks_t cmd_callbacks;
    error_cb_t error_cb;
} glw_command_setup_t;

void glw_command_setup(const glw_command_setup_t *setup);

void glw_command_run(char *input);
