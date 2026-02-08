#pragma once

#include "lua.h"
#include "glowy_script.h"

void lua_led_lib_cfg(const glowy_script_init_t *init);
void lua_led_lib_register(lua_State *L);
