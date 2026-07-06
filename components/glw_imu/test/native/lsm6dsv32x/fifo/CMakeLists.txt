set(name glw_imu_lsm6dsv32x_fifo)

set(executable_target ${name}_native_test_app)
set(test_name ${name}_native)

add_executable(${executable_target}
    test_lsm6dsv32x_fifo.c
)

target_link_libraries(${executable_target}
  imu_native_test_common
)

add_test(NAME ${test_name} COMMAND ${executable_target})
