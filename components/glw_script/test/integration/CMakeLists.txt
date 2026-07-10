set(TARGET glw_script_integration_test)

add_library(${TARGET} STATIC
  test_glw_script.c
)

target_include_directories(${TARGET}
    PUBLIC .
    PRIVATE ../../src
)

target_link_libraries(${TARGET} PRIVATE
    glw_script
    glw_os
)

link_idf_components(${TARGET} PRIVATE unity)
