set(TARGET glw_os)

set(SOURCES
    src/shared_buffers.c
)

if(NOT BUILD_NATIVE)
    link_idf_components(${TARGET} PUBLIC freertos)
endif()

glw_add_module(${TARGET}
    SOURCES "${SOURCES}"
)

if(BUILD_NATIVE)
    target_include_directories(${TARGET} BEFORE PRIVATE test/native/mock)
    target_link_libraries(${TARGET} PRIVATE fff)
endif()