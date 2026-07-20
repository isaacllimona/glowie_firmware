set(TARGET glw_led)

set(SOURCES
    src/glw_led.c
)

if(NOT BUILD_NATIVE)
    list(APPEND SOURCES "src/driver/driver_ws2812.c")
endif()

glw_add_module(${TARGET}
    SOURCES "${SOURCES}"
    INCLUDE_PRIVATE src
    DEPENDS glw_log
)

if(BUILD_NATIVE)
    add_subdirectory(test/native/mock)
    target_link_libraries(${TARGET} PRIVATE glw_led_driver_mock)
else()
    add_subdirectory(led_strip)
    target_link_libraries(${TARGET} PRIVATE led_strip)
endif()
