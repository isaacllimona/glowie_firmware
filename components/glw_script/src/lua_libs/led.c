#include <assert.h>

#include "lauxlib.h"

#include "glw_script_priv.h"
#include "lua.h"

#include "glw_script_libs/led.h"

static void (*set_color_rgb) (glw_script_libs_led_rgb_comp_t r, glw_script_libs_led_rgb_comp_t g, glw_script_libs_led_rgb_comp_t b);
static void (*set_pixel_color_rgb)(glw_script_libs_led_rgb_comp_t r, glw_script_libs_led_rgb_comp_t g, glw_script_libs_led_rgb_comp_t b, int idx);
static void (*update)(void);

// ===== LUA function helpers =====

static int lua_check_integer_at_index(lua_State *L, int table_idx, int element_idx) {
    lua_geti(L, table_idx, element_idx);
    luaL_checkinteger(L, -1);
}

// ===== LUA library funcions =====

static int lua_setColor(lua_State *L) {
    int r, g, b;
    if (lua_istable(L, 1)) {
        // Color is given as a table {<r>, <g>, <b>}
        r = lua_check_integer_at_index(L, 1, 1);
        g = lua_check_integer_at_index(L, 1, 2);
        b = lua_check_integer_at_index(L, 1, 3);
    }
    else {
        // Color is given as 3 separate args
        r = luaL_checkinteger(L, 1);
        g = luaL_checkinteger(L, 2);
        b = luaL_checkinteger(L, 3);
    }

    set_color_rgb(r, g, b);
    return 0;
}

static int lua_setPixelColor(lua_State *L) {
    int r, g, b, pixel;
    if (lua_istable(L, 1)) {
        // Color is given as a table {<r>, <g>, <b>}
        r = lua_check_integer_at_index(L, 1, 1);
        g = lua_check_integer_at_index(L, 1, 2);
        b = lua_check_integer_at_index(L, 1, 3);
        pixel = luaL_checkinteger(L, 2);
    }
    else {
        // Color is given as 3 separate args
        r = luaL_checkinteger(L, 1);
        g = luaL_checkinteger(L, 2);
        b = luaL_checkinteger(L, 3);
        pixel = luaL_checkinteger(L, 4);
    }

    set_pixel_color_rgb(r, g, b, pixel);
    return 0;
}

static int lua_update(lua_State *L) {
    update();
    return 0;
}

// =========================

static const luaL_Reg lib_funcs[] = {
    {"setColor", lua_setColor},
    {"setPixelColor", lua_setPixelColor},
    {"update", lua_update},
    {NULL, NULL}
};

void glw_script_libs_led_add(const glw_script_libs_led_init_t *init) {
    assert(init->set_color_rgb != NULL);
    assert(init->set_pixel_color_rgb != NULL);
    assert(init->update != NULL);

    set_color_rgb = init->set_color_rgb;
    set_pixel_color_rgb = init->set_pixel_color_rgb;
    update = init->update;

    add_lua_library(lib_funcs, "led");
}
