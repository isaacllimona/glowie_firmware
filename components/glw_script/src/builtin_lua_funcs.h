#pragma once

#include "lua.h"
#include "glw_script.h"

void builtin_lua_funcs_init(const glw_script_init_t *init);
void builtin_lua_funcs_register(lua_State *L);
