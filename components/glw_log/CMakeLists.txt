set(TARGET glw_log)

glw_add_module(${TARGET}
    SOURCES
        src/dummy.c
)

link_idf_components(${TARGET} PUBLIC log)
