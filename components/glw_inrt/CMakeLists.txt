glw_exclude_from_native_build()

set(TARGET glw_inrt)

set(SOURCES
    src/glw_inrt.c
    src/state_estimation.c
    src/data_dispatcher.c
    src/imu/imu_bmi160.c
    src/task.c
)

glw_add_module(${TARGET}
    SOURCES ${SOURCES}
    DEPENDS glw_log glw_os bmi160 glw_imu
)
