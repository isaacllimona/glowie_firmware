set(TARGET glw_comms)

glw_add_module(${TARGET}
    SOURCES
        src/glw_comms.c
        src/gap/gap.c
        src/gatt/gatt.c
    DEPENDS glw_log
)

link_idf_components(${TARGET} PRIVATE bt)
