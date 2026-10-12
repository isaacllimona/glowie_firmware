add_executable(glw_os_native_test_app
    test_shared_buffers.c
    mutex_mock.c
)

target_include_directories(glw_os_native_test_app BEFORE PRIVATE
    mock
)

target_link_libraries(glw_os_native_test_app
    native_test_common
    glw_os
)

add_test(NAME glw_os_native COMMAND glw_os_native_test_app)