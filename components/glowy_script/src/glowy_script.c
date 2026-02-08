#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "log.h"
#include "lua_libs/lua_led_lib.h"
#include "lua_libs/lua_sys_lib.h"
#include "glowy_script_priv.h"


static void register_libraries(lua_State *L) {
    lua_led_lib_register(L);
    lua_sys_lib_register(L);
}

static void script_loop(lua_State *L) {
    while(true) {
        if(wait_and_load_input(L, max_timeout)) {
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

    LOGI("Registering glowy libraries to LUA...");
    register_libraries(L);

    script_loop(L);
}

bool wait_and_load_input(lua_State *L, glowy_script_time_t timeout) {
    char script_buffer[INPUT_BUFFER_SIZE];
    if (wait_for_input(script_buffer, timeout)) {
        luaL_loadstring(L, script_buffer);
        return true;
    }
    return false;
}
