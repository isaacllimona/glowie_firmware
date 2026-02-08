#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "log.h"
#include "lua_libs/lua_led_lib.h"
#include "lua_libs/lua_sys_lib.h"
#include "glowy_script.h"

static lua_State *L;

void run_script(char * script) {
    lua_State * new_thread = lua_newthread(L);
    luaL_loadstring(new_thread, script);
    int error = luaL_dostring(L, script);

    if (error != LUA_OK) {
        LOGE("Error running script: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static void setup_libraries(const glowy_script_init_t *init){
    lua_led_lib_cfg(init);
    lua_sys_lib_cfg(init);
}

static void register_libraries(lua_State *L) {
    lua_led_lib_register(L);
    lua_sys_lib_register(L);
}

void glowy_script_init(const glowy_script_init_t *init) {
    LOGD("Creating LUA state...");

    L = luaL_newstate();
    assert(L != NULL);
    luaL_openlibs(L);

    LOGD("Registering glowy libraries to LUA...");

    setup_libraries(init);
    register_libraries(L);
}
