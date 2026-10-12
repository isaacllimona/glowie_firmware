#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "log.h"
#include "builtin_lua_funcs.h"
#include "glw_script_priv.h"
#include "glw_script_task.h"
#include "glw_script.h"

static const char *stop_error_msg = "Stopped";

static void (*output_error_cb)(const char *msg);
static void output_error(const char *msg) { output_error_cb(msg); }

static lua_State *L;

static bool wait_and_load_input(lua_State *L, glw_script_time_t timeout) {
    char *script = wait_for_input(timeout);
    if (script != NULL) {
        luaL_loadstring(L, script);
        release_input();
        return true;
    }
    return false;
}

static void script_loop(lua_State *L) {
    while (true) {
        if (wait_and_load_input(L, max_timeout)) {
            int error = lua_pcall(L, 0, 0, 0);
            if (error != LUA_OK) {
                const char *err_msg = lua_tostring(L, -1);
                // Do not output the error message on the error raised by script_stop()
                if (strcmp(stop_error_msg, err_msg) != 0) {
                    output_error(err_msg);
                }
                lua_pop(L, 1);
            }
        }
    }
}

void run_interpreter() {
    LOGI("Creating LUA state...");
    L = luaL_newstate();
    assert(L != NULL);
    luaL_openlibs(L);

    LOGI("Registering glowie libraries to LUA...");
    builtin_lua_funcs_register(L);

    script_loop(L);
}

void add_lua_library(const luaL_Reg *lib_funcs, const char *name) {
    lua_newtable(L);
    luaL_setfuncs(L, lib_funcs, 0);
    lua_setglobal(L, name);
}

void script_stop(lua_State *L) {
    // Stop the running script by raising an error
    lua_pushstring(L, stop_error_msg);
    lua_error(L);
}

void script_sleep(lua_State *L, int time) {
    glw_script_time_t end_time = get_time_ms() + time;

    glw_script_time_t timeout = time;
    while (timeout > 0 && wait_and_load_input(L, timeout)) {
        lua_call(L, 0, 0);
        timeout = end_time - get_time_ms();
    }
}

// ======== PUBLIC API =========

void glw_script_init(const glw_script_init_t *init) {
    assert (init->error_cb != NULL);
    output_error_cb = init->error_cb;

    builtin_lua_funcs_init(init);
}

void glw_script_start(void) {
    script_task_start(run_interpreter);
}

void glw_script_stop(void) {
    script_task_cancel();
}
