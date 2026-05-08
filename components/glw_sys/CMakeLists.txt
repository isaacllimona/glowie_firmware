idf_component_register(
    SRCS "src/glw_sys.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    PRIV_REQUIRES glw_common esp_driver_gpio
)
