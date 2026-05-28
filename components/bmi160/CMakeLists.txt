glw_exclude_from_native_build()

# This allows this module to be discovered as an idf component
if(DEFINED COMPONENT_TARGET)
    set(TARGET ${COMPONENT_TARGET})
else()
    set(TARGET bmi160)
endif()

set(SOURCES
    src/bmi160.c
    src/spi_interface.c
    src/fifo.c
)

glw_add_module(${TARGET}
    SOURCES "${SOURCES}"
    DEPENDS glw_os glw_log
)

link_idf_components(${TARGET} PRIVATE esp_driver_spi)
