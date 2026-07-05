add_executable(glw_imu_native_test_app
    test_lsm6dsv32x.c
)

target_link_libraries(glw_imu_native_test_app
  native_test_common
  glw_imu
)

add_test(NAME glw_imu_native_tests COMMAND glw_imu_native_test_app)
