function(glw_add_module module_name)
    set(multiValueArgs SOURCES INCLUDE INCLUDE_PRIVATE DEPENDS)
    cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_INCLUDE)
        set(ARG_INCLUDE include)
    endif()

    if(NOT ARG_INCLUDE_PRIVATE)
        set(ARG_INCLUDE_PRIVATE src)
    endif()

    message(${CMAKE_CURRENT_SOURCE_DIR})

    add_library(${module_name} STATIC ${ARG_SOURCES})
    target_include_directories(${module_name} PUBLIC ${ARG_INCLUDE} PRIVATE ${ARG_INCLUDE_PRIVATE})
    target_link_libraries(${module_name} PRIVATE ${ARG_DEPENDS})

    if(BUILD_TESTING AND EXISTS test/CMakeLists.txt)
        add_subdirectory(test)
    endif()
endfunction()
