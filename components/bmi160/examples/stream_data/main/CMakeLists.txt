set(TARGET ${COMPONENT_TARGET})

add_library(${TARGET} STATIC
    src/main.c
    src/imu_read.c
    src/tcp_server.c
)

target_link_libraries(${TARGET} PRIVATE bmi160)

link_idf_components(${TARGET} PRIVATE freertos log esp_wifi nvs_flash)
