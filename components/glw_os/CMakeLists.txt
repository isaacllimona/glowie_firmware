set(TARGET glw_os)

add_library(${TARGET} STATIC
    src/dummy.c
)

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE src
)

link_idf_components(${TARGET} PUBLIC freertos)
