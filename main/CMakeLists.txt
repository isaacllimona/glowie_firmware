glw_exclude_from_native_build()

set(TARGET main)

add_library(${TARGET} STATIC
    main.c
    sync_clock.c
)

target_link_libraries(${TARGET} PRIVATE
    glw_log
    glw_comms
    glw_led
    glw_sys
    glw_script
    glw_os
)

link_idf_components(${TARGET} PRIVATE nvs_flash)
