idf_component_register(
    SRCS "src/main.c" "src/imu_read.c" "src/tcp_server.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES bmi160 esp_wifi nvs_flash
)
