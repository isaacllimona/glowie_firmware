add_executable(glw_script_queue_native_test_app
    test_glw_script_queue.c
)

target_include_directories(glw_script_queue_native_test_app PRIVATE
    ../../../src
)

target_link_libraries(glw_script_queue_native_test_app
    native_test_common
    glw_script
)

add_test(NAME glw_script_queue_native COMMAND glw_script_queue_native_test_app)
