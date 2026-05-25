# Build the led_strip component
add_subdirectory(led_strip)

set(TARGET glw_led)

add_library(${TARGET} STATIC
    src/glw_led.c
)

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE src
)

target_link_libraries(${TARGET} PRIVATE
    led_strip
    glw_log
)
