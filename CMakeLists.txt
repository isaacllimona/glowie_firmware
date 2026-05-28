cmake_minimum_required(VERSION 3.22)

if(NOT BUILD_NATIVE)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
endif()

project(glowie C CXX ASM)

include(tools/buildsystem/cmake/idf_utils.cmake)
include(tools/buildsystem/cmake/modules.cmake)

include(CTest)

if(NOT BUILD_NATIVE)
    idf_project_default()
else()
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
endif()

add_subdirectory(components/bmi160)
add_subdirectory(components/glw_os)
add_subdirectory(components/glw_log)
add_subdirectory(components/glw_comms)
add_subdirectory(components/glw_inrt)
add_subdirectory(components/glw_led)
add_subdirectory(components/glw_script)
add_subdirectory(components/glw_sys)
add_subdirectory(components/glw_commands)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
