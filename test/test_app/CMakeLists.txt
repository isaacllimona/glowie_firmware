add_library(test_app_main STATIC main.c)

target_link_libraries(test_app_main PRIVATE
    glw_imu_integration_test
)

link_idf_components(test_app_main PRIVATE unity)
