# Build the led_strip component
add_subdirectory(led_strip)

set(TARGET glw_led)

glw_add_module(${TARGET}
    SOURCES
        src/glw_led.c
    DEPENDS glw_log led_strip
)
