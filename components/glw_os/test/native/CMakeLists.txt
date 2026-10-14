## Add mock ##

add_subdirectory(mock)

## Create test app ##

add_executable(glw_os_native_test_app
    test_shared_buffers.c
)

target_link_libraries(glw_os_native_test_app
    native_test_common
    glw_os
)

add_test(NAME glw_os_native_test COMMAND glw_os_native_test_app)
