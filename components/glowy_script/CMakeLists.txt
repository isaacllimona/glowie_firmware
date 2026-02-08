set(lua_srcs
    "lua/lapi.c"
    "lua/lauxlib.c"
    "lua/lbaselib.c"
    "lua/lcode.c"
    "lua/lctype.c"
    "lua/lcorolib.c"
    "lua/ldblib.c"
    "lua/ldebug.c"
    "lua/ldo.c"
    "lua/lfunc.c"
    "lua/lgc.c"
    "lua/linit.c"
    "lua/liolib.c"
    "lua/llex.c"
    "lua/lmathlib.c"
    "lua/lmem.c"
    "lua/lopcodes.c"
    "lua/loadlib.c"
    "lua/loslib.c"
    "lua/lstate.c"
    "lua/ltable.c"
    "lua/ltm.c"
    "lua/lvm.c"
    "lua/lobject.c"
    "lua/lparser.c"
    "lua/lstrlib.c"
    "lua/lstring.c"
    "lua/ltablib.c"
    "lua/ldump.c"
    "lua/lundump.c"
    "lua/lutf8lib.c"
    "lua/lzio.c"
)

set(lua_libs
    "src/lua_libs/lua_led_lib.c"
    "src/lua_libs/lua_sys_lib.c"
)

idf_component_register(
    SRCS ${lua_srcs} ${lua_libs} "src/glowy_script.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "lua" "src"
)

add_compile_definitions(LUA_32BITS=1)
