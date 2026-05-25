set(TARGET glw_sys)

add_library(${TARGET} STATIC
    src/glw_sys.c
)

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE src
)

target_link_libraries(${TARGET} PRIVATE
    glw_log
)

link_idf_components(${TARGET} PRIVATE esp_driver_gpio)
