#pragma once

#include <stdbool.h>

#include "config.h"
#include "lua.h"

#define INPUT_BUFFER_SIZE CONFIG_SCRIPT_INPUT_BUFFER_SIZE

typedef unsigned int glw_script_time_t;
static const glw_script_time_t max_timeout = 0xffffffffU;

glw_script_time_t get_time_ms(void);

bool wait_for_input(char *script, glw_script_time_t timeout);

void run_interpreter(void);

void script_sleep(lua_State *L, int time);
