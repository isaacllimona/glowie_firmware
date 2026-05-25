set(TARGET glw_inrt)

set(SOURCES
    src/glw_inrt.c
    src/state_estimation.c
    src/data_dispatcher.c
    src/imu/imu_bmi160.c
    src/task.c
)

add_library(${TARGET} STATIC ${SOURCES})

target_include_directories(${TARGET}
    PUBLIC include
    PRIVATE src
)

target_link_libraries(${TARGET} PRIVATE
    bmi160
    glw_os
    glw_log
)
