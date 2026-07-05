set(TARGET glw_log)

if(BUILD_NATIVE)
    set(INCLUDE_DIR test/native/include)
else()
    set(INCLUDE_DIR include)
endif()

glw_add_module(${TARGET}
    SOURCES
        src/dummy.c
    INCLUDE ${INCLUDE_DIR}
)

if(NOT BUILD_NATIVE)
    link_idf_components(${TARGET} PUBLIC log)
endif()
