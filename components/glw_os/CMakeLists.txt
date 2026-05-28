glw_exclude_from_native_build()

set(TARGET glw_os)

glw_add_module(${TARGET}
    SOURCES
        src/dummy.c
)

link_idf_components(${TARGET} PUBLIC freertos)
