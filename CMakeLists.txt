cmake_minimum_required(VERSION 3.22)

include(tools/buildsystem/cmake/esp_idf_wrapper.cmake)
include(tools/buildsystem/cmake/modules.cmake)

project(glowie C CXX ASM)

include(CTest)

if(NOT BUILD_NATIVE)
    glw_idf_init()
else()
    add_compile_options(-Wall)
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
    add_compile_definitions(BUILD_TESTING)
endif()

add_subdirectory(main)
add_subdirectory(components/bmi160)
add_subdirectory(components/glw_os)
add_subdirectory(components/glw_log)
add_subdirectory(components/glw_comms)
add_subdirectory(components/glw_inrt)
add_subdirectory(components/glw_led)
add_subdirectory(components/glw_script)
add_subdirectory(components/glw_sys)
add_subdirectory(components/glw_commands)
add_subdirectory(components/glw_imu)

if(NOT BUILD_NATIVE)
    glw_idf_add_main_app(main)
    glw_idf_finish()
endif()
