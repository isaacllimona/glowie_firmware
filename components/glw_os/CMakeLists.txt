set(TARGET glw_os)

set(SOURCES
    src/shared_buffers.c
)

glw_add_module(${TARGET}
    SOURCES "${SOURCES}"
)

if(NOT BUILD_NATIVE)
    link_idf_components(${TARGET} PUBLIC freertos)
endif()
