set(TARGET led_strip)

set(srcs 
    "led_strip/src/led_strip_api.c"
    "led_strip/src/led_strip_rmt_dev.c"
    "led_strip/src/led_strip_rmt_encoder.c"
    "led_strip/src/led_strip_spi_dev.c"
)

add_library(${TARGET} STATIC ${srcs})
target_include_directories(${TARGET}
    PUBLIC led_strip/include led_strip/interface
)

link_idf_components(${TARGET} PUBLIC esp_driver_rmt esp_driver_spi)
