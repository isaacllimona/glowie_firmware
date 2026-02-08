#include "glowy_script.h"
#include "lauxlib.h"
#include "lua.h"

#include "log.h"
#include "lua_sys_lib.h"

void (*delay_func)(glowy_script_time_t time_ms);

// ==== LUA FUNCTIONS ====

static int lua_sleep(lua_State *L) {
    int time = luaL_checkinteger(L, 1);
    delay_func(time);
    return 0;
}

// =======================

void lua_sys_lib_cfg(const glowy_script_init_t *init) {
    delay_func = init->delay;
}

static const struct luaL_Reg lib_funcs[] = {
    {"sleep", lua_sleep},
    {NULL, NULL}
};

void lua_sys_lib_register(lua_State *L) {
    luaL_newlib(L, lib_funcs);
    lua_setglobal(L, "sys");
}
