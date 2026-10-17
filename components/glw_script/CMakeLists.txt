# lua target

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

add_library(lua STATIC ${lua_srcs})

target_include_directories(lua PUBLIC lua)

target_compile_definitions(lua PRIVATE LUA_32BITS=1)

if(BUILD_NATIVE)
    target_link_libraries(lua PRIVATE m)
endif()

# glw_script target

set(TARGET glw_script)

set(SOURCES
    src/glw_script_queue.c
    src/glw_script_lua.c
    src/lua_libs/led.c
)

if(NOT BUILD_NATIVE)
    list(APPEND SOURCES src/glw_script.c)
endif()

add_library(${TARGET} STATIC ${SOURCES})

target_include_directories(${TARGET} PUBLIC include PRIVATE src)

target_link_libraries(${TARGET}
    PUBLIC glw_os
    PRIVATE
        glw_log
        lua
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
