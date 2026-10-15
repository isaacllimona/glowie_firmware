set(TARGET glw_comms_commands)

add_library(${TARGET} STATIC
    src/glw_comms_commands.c
)

target_include_directories(${TARGET}
PUBLIC
    include
    ../../include  # To get access to the callbacks struct from the glw_comms header
PRIVATE
    src
)

target_link_libraries(${TARGET} PRIVATE glw_log)
