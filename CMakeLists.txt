cmake_minimum_required(VERSION 3.22)

include(tools/buildsystem/cmake/esp_idf_wrapper.cmake)
include(tools/buildsystem/cmake/modules.cmake)

project(glowie C CXX ASM)

include(CTest)

if(NOT BUILD_NATIVE)
    glw_idf_init()
else()
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
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

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

if(NOT BUILD_NATIVE)
    glw_idf_add_main_app(main)
    glw_idf_finish()
endif()
