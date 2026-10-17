#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "glw_os/time.h"
#include "glw_os/shared_buffers.h"

#include "log.h"
#include "glw_script_queue.h"

// implements
#include "glw_script_lua.h"

static const char *stop_error_msg = "Stopped";

static glw_script_lua_init_t init_cfg;
static lua_State *L;

static void output_error(const char *msg) { init_cfg.error_cb(msg); }

// ---- load a shared buffer into lua
static const char *lua_buffer_reader(lua_State *L, void *data, size_t *size) {
    shared_buffer_reader *reader = data;
    unsigned int length;
    const char *chunk = shared_buffer_reader_read(reader, &length);
    *size = length;
    return chunk;
}

static int load_shared_buffer(lua_State *L, shared_buffer_handle buffer) {
    shared_buffer_reader reader = shared_buffer_reader_create(buffer);
    return lua_load(L, lua_buffer_reader, &reader, "", "t");
}
// -------------------------

static bool wait_and_load_input(lua_State *L, unsigned int timeout) {
    shared_buffer_handle input_buffer = glw_script_queue_wait(timeout);

    if (input_buffer == NULL) return false;

    int ret = load_shared_buffer(L, input_buffer);
    if (ret == LUA_ERRSYNTAX) output_error("Syntax error in loaded script");
    else if (ret == LUA_ERRMEM) output_error("Memory allocation error while loading script");

    shared_buffer_release(input_buffer);

    return ret == LUA_OK;
}

void script_stop(lua_State *L) {
    // Stop the running script by raising an error
    lua_pushstring(L, stop_error_msg);
    lua_error(L);
}

void script_sleep(lua_State *L, int time) {
    unsigned int end_time = os_time() + time;

    unsigned timeout = time;
    while (timeout > 0 && wait_and_load_input(L, timeout)) {
        lua_call(L, 0, 0);
        timeout = end_time - os_time();
    }
}

// ==== LUA BUILTIN FUNCTIONS ====

/** Waits for the given time in ms */
static int lua_sleep(lua_State *L) {
    int sleep_time = luaL_checkinteger(L, 1);
    script_sleep(L, sleep_time);
    return 0;
}

/** Stop the running script */
static int lua_stop(lua_State *L) {
    script_stop(L);
    return 0;
}

static int lua_time(lua_State *L) {
    unsigned int time = init_cfg.get_sync_time();
    // As LUA integers are 32 bits, bad things may happen if time does not fit in a signed int
    // Then again a signed int could run for many days so that should't be an issue
    lua_pushinteger(L, time);
    return 1;
}

// ========================

static void add_lua_func(lua_State *L, int (*func)(lua_State *), const char *name) {
    lua_pushcfunction(L, func);
    lua_setglobal(L, name);
}

static void register_lua_funcs(lua_State *L) {
    add_lua_func(L, lua_sleep, "sleep");
    add_lua_func(L, lua_stop, "stop");
    add_lua_func(L, lua_time, "time");
}

// ------ PUBLIC FUNCTIONS ------

void glw_script_lua_init(const glw_script_lua_init_t *init) {
    init_cfg = *init;

    LOGI("Creating LUA state...");
    L = luaL_newstate();
    assert(L != NULL);
    luaL_openlibs(L);

    LOGI("Registering glowie functions to LUA...");
    register_lua_funcs(L);
}

void glw_script_lua_run(void) {
    assert(L != NULL);
    while (true) {
        if (wait_and_load_input(L, 0xFFFFFFFF)) {
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

void glw_script_lua_add_library(const luaL_Reg *lib_funcs, const char *name) {
    lua_newtable(L);
    luaL_setfuncs(L, lib_funcs, 0);
    lua_setglobal(L, name);
}
