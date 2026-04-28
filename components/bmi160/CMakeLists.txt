idf_component_register(
    SRCS "src/bmi160.c" "src/spi_interface.c" "src/fifo.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_driver_spi glw_common
)
