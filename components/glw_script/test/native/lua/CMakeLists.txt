add_executable(glw_script_lua_native_test_app
    test_glw_script_lua.c
)

target_include_directories(glw_script_lua_native_test_app PRIVATE
    ../../../src
)

target_link_libraries(glw_script_lua_native_test_app
PRIVATE
    native_test_common
    glw_log
    glw_os
    lua
    glw_script
)

add_test(NAME glw_script_lua_native COMMAND glw_script_lua_native_test_app)
