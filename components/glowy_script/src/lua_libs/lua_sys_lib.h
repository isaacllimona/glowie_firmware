#pragma once

#include "glowy_script.h"
#include "lua.h"

void lua_sys_lib_cfg(const glowy_script_init_t *init);
void lua_sys_lib_register(lua_State *L);
