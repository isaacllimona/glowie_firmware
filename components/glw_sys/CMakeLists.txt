set(TARGET glw_sys)

glw_add_module(${TARGET}
    SOURCES
        src/glw_sys.c
    DEPENDS glw_log
)

link_idf_components(${TARGET} PRIVATE esp_driver_gpio)
