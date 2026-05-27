cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(glowie C CXX ASM)

include(tools/buildsystem/cmake/idf_utils.cmake)
include(tools/buildsystem/cmake/modules.cmake)

idf_project_default()

add_subdirectory(components/bmi160)
add_subdirectory(components/glw_os)
add_subdirectory(components/glw_log)
add_subdirectory(components/glw_comms)
add_subdirectory(components/glw_inrt)
add_subdirectory(components/glw_led)
add_subdirectory(components/glw_script)
add_subdirectory(components/glw_sys)
