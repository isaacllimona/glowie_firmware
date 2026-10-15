add_subdirectory(libs/BLE)
add_subdirectory(libs/commands)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

if(BUILD_NATIVE)
    return()
endif()

set(TARGET glw_comms)

add_library(${TARGET} STATIC
    src/glw_comms.c
)

target_include_directories(${TARGET}
PUBLIC
    include
PRIVATE
    src
)

target_link_libraries(${TARGET} PRIVATE
    glw_log glw_os
    glw_comms_ble glw_comms_commands
)
