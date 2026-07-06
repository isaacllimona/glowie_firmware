set(TARGET glw_imu)

set(SOURCES
    src/lsm6dsv32x/glw_imu.c
    src/lsm6dsv32x/fifo.c
    third_party/lsm6dsv32x/lsm6dsv32x_reg.c
)

if(BUILD_NATIVE)
    list(APPEND SOURCES test/native/bus_mock.c)
else()
    list(APPEND SOURCES src/bus/bus_i2c.c)
endif()

glw_add_module(${TARGET}
    SOURCES "${SOURCES}"
    INCLUDE_PRIVATE third_party/lsm6dsv32x src
    DEPENDS glw_log
)

if(BUILD_NATIVE)
    target_link_libraries(${TARGET} PRIVATE fff)
endif()

link_idf_components(${TARGET} PRIVATE esp_driver_spi esp_driver_i2c esp_driver_gpio)
