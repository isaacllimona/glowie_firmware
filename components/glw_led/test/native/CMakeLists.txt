set(name glw_led)

set(executable_target ${name}_native_test_app)
set(test_name ${name}_native)

add_executable(${executable_target}
    test_glw_led.c
)

target_link_libraries(${executable_target}
    native_test_common
    glw_led_driver_mock
    glw_led
)

add_test(NAME ${test_name} COMMAND ${executable_target})
