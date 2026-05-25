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

add_library(${TARGET} STATIC ${SOURCES})

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE src
)

target_link_libraries(${TARGET} PRIVATE
    glw_os
    glw_log
)

link_idf_components(${TARGET} PRIVATE esp_driver_spi)
