#include "glowy_script.h"
#include "lauxlib.h"
#include "lua.h"

#include "log.h"
#include "glowy_script_priv.h"
#include "lua_sys_lib.h"


// ==== LUA FUNCTIONS ====

/** Waits for the given time in ms */
static int lua_sleep(lua_State *L) {
    int sleep_time = luaL_checkinteger(L, 1);
    glowy_script_time_t end_time = get_time_ms() + sleep_time;

    glowy_script_time_t timeout = sleep_time;
    while (timeout > 0 && wait_and_load_input(L, timeout)) {
        lua_call(L, 0, 0);
        timeout = end_time - get_time_ms();
    }
    return 0;
}

/** Stop the running script by raising an error */
static int lua_stop(lua_State *L) {
    return luaL_error(L, "Stopped");
}

// =======================

static const struct luaL_Reg lib_funcs[] = {
    {"sleep", lua_sleep},
    {"stop", lua_stop},
    {NULL, NULL}
};

void lua_sys_lib_register(lua_State *L) {
    luaL_newlib(L, lib_funcs);
    lua_setglobal(L, "sys");
}
