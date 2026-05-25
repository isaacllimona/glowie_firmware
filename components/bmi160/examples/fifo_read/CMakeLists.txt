cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

set(project example_bmi160_fifo_read)

project(${project} C CXX ASM)

set(glowie_project_dir "../../../..") 
include(${glowie_project_dir}/tools/buildsystem/cmake/idf_utils.cmake)

idf_project_default()

set(libs_dir "../../..")
add_subdirectory(${libs_dir}/bmi160  build/libs/bmi160)
add_subdirectory(${libs_dir}/glw_os  build/libs/glw_os)
add_subdirectory(${libs_dir}/glw_log build/libs/glw_log)
