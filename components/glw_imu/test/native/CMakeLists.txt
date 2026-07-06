add_subdirectory(lsm6dsv32x/imu)
add_subdirectory(lsm6dsv32x/fifo)

add_library(imu_native_test_common INTERFACE)
target_include_directories(imu_native_test_common INTERFACE
    .           # To access the bus mock
    ../../src   # To access glw_imu privates
)
target_link_libraries(imu_native_test_common INTERFACE
    native_test_common
    glw_imu
)
