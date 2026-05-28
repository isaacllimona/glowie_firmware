glw_exclude_from_native_build()

set(TARGET glw_script)

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

glw_add_module(${TARGET}
    SOURCES
        ${lua_srcs}
        ${lua_libs}
        src/glw_script.c
        src/glw_script_freertos.c
    INCLUDE_PRIVATE lua src
    DEPENDS glw_log
)

if(NOT BUILD_NATIVE)
    target_compile_definitions(${TARGET} PRIVATE LUA_32BITS=1)
endif()

link_idf_components(${TARGET} PRIVATE freertos)
