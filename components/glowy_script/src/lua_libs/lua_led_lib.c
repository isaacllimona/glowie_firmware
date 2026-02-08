#include "glowy_script.h"
#include "lauxlib.h"
#include "lua.h"

#include "log.h"
#include "lua_led_lib.h"

static void (*set_color_rgb)(uint8_t r, uint8_t g, uint8_t b);
static void (*set_color_hsv)(uint8_t h, uint8_t s, uint8_t v);

// ==== LUA FUNCTIONS ====

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

static const struct luaL_Reg lib_funcs[] = {
    {"setColorRGB", lua_setColorRGB},
    {"setColorHSV", lua_setColorHSV},
    {NULL, NULL}
};

// =======================

void lua_led_lib_cfg(const glowy_script_init_t *init) {
    set_color_rgb = init->set_color_rgb;
    set_color_hsv = init->set_color_hsv;
}

void lua_led_lib_register(lua_State *L) {
    luaL_newlib(L, lib_funcs);
    lua_setglobal(L, "led");
}
