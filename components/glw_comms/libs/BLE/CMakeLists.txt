glw_exclude_from_native_build()

set(TARGET glw_comms_ble)

add_library(${TARGET} STATIC
    src/glw_comms_ble.c
    src/gap/gap.c
    src/gatt/gatt.c
)

target_include_directories(${TARGET}
PUBLIC
    include
PRIVATE
    src
)

target_link_libraries(${TARGET} PRIVATE glw_log)

link_idf_components(${TARGET} PRIVATE bt)
