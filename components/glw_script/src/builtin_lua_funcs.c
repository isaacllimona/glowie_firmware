#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "glw_script_priv.h"

// ---- injected dependencies
static void (*set_color_rgb)(uint8_t r, uint8_t g, uint8_t b);
static void (*set_color_hsv)(uint8_t h, uint8_t s, uint8_t v);

// ==== LUA FUNCTIONS ====

/** Waits for the given time in ms */
static int lua_sleep(lua_State *L) {
    int sleep_time = luaL_checkinteger(L, 1);
    script_sleep(L, sleep_time);
    return 0;
}

/** Stop the running script by raising an error */
static int lua_stop(lua_State *L) { return luaL_error(L, "Stopped"); }

static int lua_setColorRGB(lua_State *L) {
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);
    set_color_rgb(r, g, b);
    return 0;
}

static int lua_setColorHSV(lua_State *L) {
    int h = luaL_checkinteger(L, 1);
    int s = luaL_checkinteger(L, 2);
    int v = luaL_checkinteger(L, 3);
    set_color_hsv(h, s, v);
    return 0;
}

// ========================

void builtin_lua_funcs_init(const glw_script_init_t *init) {
    set_color_rgb = init->set_color_rgb;
    set_color_hsv = init->set_color_hsv;
}

static void add_lua_func(lua_State *L, int (*func)(lua_State *), const char *name) {
    lua_pushcfunction(L, func);
    lua_setglobal(L, name);
}

void builtin_lua_funcs_register(lua_State *L) {
    // Assert all callbacks have been properly set up before registration
    assert(set_color_rgb != NULL);
    assert(set_color_hsv != NULL);

    add_lua_func(L, lua_sleep, "sleep");
    add_lua_func(L, lua_stop, "stop");
    add_lua_func(L, lua_setColorRGB, "setColorRGB");
    add_lua_func(L, lua_setColorHSV, "setColorHSV");
}
