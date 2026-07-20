#pragma once

#include <stdbool.h>

#include "lua.h"
#include "lauxlib.h"

void run_interpreter(void);

void add_lua_library(const luaL_Reg *lib_funcs, const char *name);

void script_stop(lua_State *L);

void script_sleep(lua_State *L, int time);
