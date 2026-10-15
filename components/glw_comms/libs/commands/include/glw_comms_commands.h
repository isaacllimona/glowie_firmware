#pragma once

#include "glw_comms.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CMD_ERR_OK = 0,
    CMD_ERR_UNKNOWN_COMMAND,
    CMD_ERR_BAD_ARGUMENTS,
} glw_comms_commands_error_t;

typedef struct {
    glw_comms_callbacks_t callbacks;
} glw_comms_commands_init_t;

void glw_comms_commands_init(glw_comms_commands_init_t setup);

glw_comms_commands_error_t glw_comms_commands_run(const char *input);
