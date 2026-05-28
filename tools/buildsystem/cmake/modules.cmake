#[[
    glw_add_module
        SOURCES <paths...>              # module source files
        [INCLUDE <paths...>]            # public include paths. Defaults to "include"
        [INCLUDE_PRIVATE <paths...>]    # private include paths. Defaults to "src"
        [DEPENDS <modules...>]          # dependencies to other modules
]]#

function(glw_add_module module_name)
    set(options )
    set(multiValueArgs SOURCES INCLUDE INCLUDE_PRIVATE DEPENDS)
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED ARG_INCLUDE)
        set(ARG_INCLUDE include)
    endif()

    if(NOT DEFINED ARG_INCLUDE_PRIVATE)
        set(ARG_INCLUDE_PRIVATE src)
    endif()

    add_library(${module_name} STATIC ${ARG_SOURCES})
    target_include_directories(${module_name} PUBLIC ${ARG_INCLUDE} PRIVATE ${ARG_INCLUDE_PRIVATE})
    target_link_libraries(${module_name} PRIVATE ${ARG_DEPENDS})

    if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test")
        add_subdirectory(test)
    endif()
endfunction()


macro(glw_exclude_from_native_build)
    if(BUILD_NATIVE)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/native")
            message(WARNING "Glowie module ${module_name} has a native test directory but it is excluded from native builds.\n"
                            "Add option NATIVE to glw_add_module to include this module in native builds.")
        endif()

        return()
    endif()
endmacro()
