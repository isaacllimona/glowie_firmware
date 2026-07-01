#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "log.h"
#include "builtin_lua_funcs.h"
#include "glw_script_priv.h"
#include "glw_script.h"

static bool wait_and_load_input(lua_State *L, glw_script_time_t timeout) {
    char script_buffer[INPUT_BUFFER_SIZE];
    if (wait_for_input(script_buffer, timeout)) {
        luaL_loadstring(L, script_buffer);
        return true;
    }
    return false;
}

static void script_loop(lua_State *L) {
    while (true) {
        if (wait_and_load_input(L, max_timeout)) {
            int error = lua_pcall(L, 0, 0, 0);
            if (error != LUA_OK) {
                LOGE("Error running script: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    }
}

void run_interpreter() {
    LOGI("Creating LUA state...");
    lua_State *L = luaL_newstate();
    assert(L != NULL);
    luaL_openlibs(L);

    LOGI("Registering glowie libraries to LUA...");
    builtin_lua_funcs_register(L);

    script_loop(L);
}

void script_sleep(lua_State *L, int time) {
    glw_script_time_t end_time = get_time_ms() + time;

    glw_script_time_t timeout = time;
    while (timeout > 0 && wait_and_load_input(L, timeout)) {
        lua_call(L, 0, 0);
        timeout = end_time - get_time_ms();
    }
}

void glw_script_init(glw_script_init_t *init) { builtin_lua_funcs_init(init); }
