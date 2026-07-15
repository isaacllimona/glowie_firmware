#pragma once

#include <stdbool.h>

#include "lua.h"

void run_interpreter(void);

void script_stop(lua_State *L);

void script_sleep(lua_State *L, int time);
