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

add_library(${TARGET} STATIC
    ${lua_srcs}
    ${lua_libs}
    src/glw_script.c
    src/glw_script_freertos.c
)

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE lua src
)

target_link_libraries(${TARGET} PRIVATE
    glw_log
)

add_compile_definitions(LUA_32BITS=1)

link_idf_components(${TARGET} PRIVATE freertos)
