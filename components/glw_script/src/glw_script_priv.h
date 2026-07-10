#pragma once

#include <stdbool.h>

#include "lua.h"

void run_interpreter(void);

void script_sleep(lua_State *L, int time);
