#pragma once

#include "lauxlib.h"

typedef struct {
    unsigned int (*get_sync_time)(void);
    void (*error_cb)(const char *msg);
} glw_script_lua_init_t;

/**
 * Start the LUA interpreter
 */
void glw_script_lua_init(const glw_script_lua_init_t *init);

void glw_script_lua_run(void);

void glw_script_lua_add_library(const luaL_Reg *lib_funcs, const char *name);
