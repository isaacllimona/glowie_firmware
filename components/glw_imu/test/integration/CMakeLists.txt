set(TARGET glw_imu_integration_test)

add_library(${TARGET} STATIC
  test_glw_imu.c
)

target_include_directories(${TARGET} PUBLIC .)

target_link_libraries(${TARGET} PRIVATE
    glw_imu
    glw_os
)

link_idf_components(${TARGET} PRIVATE unity)
