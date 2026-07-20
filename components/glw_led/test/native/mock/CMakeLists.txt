add_library(glw_led_driver_mock
    driver_mock.c
)

target_include_directories(glw_led_driver_mock PUBLIC
    .
    ../../../src
)

target_link_libraries(glw_led_driver_mock PRIVATE fff)
