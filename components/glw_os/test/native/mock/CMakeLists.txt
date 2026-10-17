target_include_directories(glw_os BEFORE PUBLIC include)

target_sources(glw_os PRIVATE
    mutex_mock.c
    queue_mock.c
    time_mock.c
)

target_link_libraries(glw_os PUBLIC fff)
