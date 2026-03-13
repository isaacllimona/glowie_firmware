idf_component_register(
    SRCS "src/glowy_inrt.c" "src/state_estimation.c" "src/data_dispatcher.c" "src/imu/imu_bmi160.c" "src/task.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    PRIV_REQUIRES bmi160 glowy_common
)
