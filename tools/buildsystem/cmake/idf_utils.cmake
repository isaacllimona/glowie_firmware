function(link_idf_components target)
    if(BUILD_NATIVE)
        return()
    endif()

    cmake_parse_arguments(ARG "" "" "PUBLIC;PRIVATE" ${ARGN})

    message(DEBUG "GLOWIE: Linking components: (public:) ${ARG_PUBLIC}, (private:) ${ARG_PRIVATE} -- to target ${target}")

    if(NOT DEFINED COMPONENT_TARGET)  # If this target is not built as an IDF component
        # Add idf include paths and compile definitions, as some idf components may need them
        # Make them public if any idf component is linked with public scope

        idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
        idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)

        if(ARG_PUBLIC)
            target_include_directories(${target} PUBLIC ${include_directories})
            target_compile_definitions(${target} PUBLIC ${compile_definitions})
        else()
            target_include_directories(${target} PRIVATE ${include_directories})
            target_compile_definitions(${target} PRIVATE ${compile_definitions})
        endif()
    endif()

    set(public_component_interfaces)
    set(private_component_interfaces)

    foreach(component IN LISTS ARG_PUBLIC)
        idf_component_include(${component})
        list(APPEND public_component_interfaces "idf::${component}")
    endforeach()

    foreach(component IN LISTS ARG_PRIVATE)
        idf_component_include(${component})
        list(APPEND private_component_interfaces "idf::${component}")
    endforeach()

    message(DEBUG "GLOWIE: Linking idf components: ${private_component_interfaces} to target ${target}")

    target_link_libraries(${target} PRIVATE ${private_component_interfaces} PUBLIC ${public_component_interfaces})
endfunction()