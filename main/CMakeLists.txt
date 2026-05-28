if(DEFINED COMPONENT_TARGET)
    set(TARGET ${COMPONENT_TARGET})
else()
    set(TARGET main)
endif()

add_library(${TARGET} STATIC
    main.c
)

target_link_libraries(${TARGET} PRIVATE
    glw_log
    glw_comms
    glw_led
    glw_sys
    glw_script
    glw_commands
)

link_idf_components(${TARGET} PRIVATE nvs_flash)
