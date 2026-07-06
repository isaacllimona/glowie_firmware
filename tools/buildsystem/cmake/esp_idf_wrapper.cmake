########################################
# Wrapper for the esp-idf build system #
########################################

include_guard(GLOBAL)

if(NOT BUILD_NATIVE)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
endif()

# This is a macro as the idf_project_init() macro must be called from the global scope
macro(glw_idf_init)
    if(BUILD_NATIVE)
        return()
    endif()

    set(COMPONENT_DIRS "")  # Do not scan project for idf components

    idf_project_init()

    set_property(GLOBAL PROPERTY GLW_ALL_IDF_COMPONENTS "")
    set_property(GLOBAL PROPERTY GLW_IDF_TEST_APPS "")
endmacro()

function(link_idf_components target)
    if(BUILD_NATIVE)
        return()
    endif()

    cmake_parse_arguments(ARG "" "" "PUBLIC;PRIVATE" ${ARGN})

    message(DEBUG "${target}: Linking IDF components: (public:) ${ARG_PUBLIC}, (private:) ${ARG_PRIVATE}")

    # Add components to the global list of idf components
    set_property(GLOBAL APPEND PROPERTY GLW_ALL_IDF_COMPONENTS ${ARG_PUBLIC} ${ARG_PRIVATE})

    if(NOT DEFINED COMPONENT_TARGET)
        # If this target is not built as an IDF component
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

    foreach(component IN LISTS ARG_PUBLIC ARG_PRIVATE)
        idf_component_include(${component})
    endforeach()

    list(TRANSFORM ARG_PUBLIC PREPEND "idf::" OUTPUT_VARIABLE public_component_interfaces)
    list(TRANSFORM ARG_PRIVATE PREPEND "idf::" OUTPUT_VARIABLE private_component_interfaces)

    target_link_libraries(${target}
        PRIVATE ${private_component_interfaces}
        PUBLIC ${public_component_interfaces}
    )
endfunction()

function(glw_idf_add_main_app main_lib)
    # Addition of app targets is deferred to glw_idf_finish()
    # as the idf component library needs to be added first
    set_property(GLOBAL PROPERTY GLW_IDF_MAIN_APP_LIB ${main_lib})
endfunction()

function(glw_idf_add_test_app name main_lib)
    message(DEBUG "Adding test app: ${name}")

    add_library(${name}_interface INTERFACE)
    target_link_libraries(${name}_interface INTERFACE ${main_lib})
    set_property(GLOBAL APPEND PROPERTY GLW_IDF_TEST_APPS ${name})
endfunction()

function(__add_idf_app app_name main_lib)
    set(idf_components_lib "idf_components")

    cmake_parse_arguments(ARG "FLASH" "" "" ${ARGN})

    set(dummy_source_file "${CMAKE_CURRENT_BINARY_DIR}/null.c")
    file(WRITE ${dummy_source_file} "")
    add_executable(${app_name}.elf ${dummy_source_file})

    target_link_libraries(${app_name}.elf PRIVATE
        ${idf_components_lib}
        ${main_lib}
    )

    # Add idf compile options
    idf_build_get_compile_options(compile_options)
    target_compile_options(${app_name}.elf PRIVATE "${compile_options}")

    # Create binary and flash targets
    idf_build_binary(${app_name}.elf
        OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${app_name}.bin"
        TARGET ${app_name}_binary
    )
    idf_check_binary_size(${app_name}_binary)

    if(ARG_FLASH)
        set(flash_arg "FLASH")
    endif()
    idf_flash_binary(${app_name}_binary
        TARGET ${app_name}-flash
        NAME "${app_name}"
        ${flash_arg}
    )
endfunction()

function(__add_main_app main_lib)
    set(app_name main_app)

    __add_idf_app(${app_name} ${main_lib} FLASH)

    idf_create_menuconfig(${app_name}.elf TARGET menuconfig)
    idf_create_confserver(${app_name}.elf TARGET confserver)

    idf_build_generate_metadata(BINARY ${app_name}_binary)
    idf_build_generate_flasher_args()
endfunction()

function(__add_test_app name main_lib)
    __add_idf_app(${name} ${main_lib})
endfunction()

function(glw_idf_finish)
    # Create library with all required ESP-IDF components
    get_property(all_idf_components GLOBAL PROPERTY GLW_ALL_IDF_COMPONENTS)
    list(REMOVE_DUPLICATES all_idf_components)
    message("Adding idf library with components: ${all_idf_components}")
    idf_build_library(idf_components COMPONENTS ${all_idf_components})

    # Create main app
    message(DEBUG "Adding targets for main idf app")
    get_property(main_app_lib GLOBAL PROPERTY GLW_IDF_MAIN_APP_LIB)
    if(NOT main_app_lib)
        message(WARNING "No main idf app has been added. Add it with glw_idf_add_main_app()")
    else()
        __add_main_app(${main_app_lib})
    endif()

    # Create test apps
    get_property(test_apps GLOBAL PROPERTY GLW_IDF_TEST_APPS)
    foreach(test_app IN LISTS test_apps)
        message(DEBUG "Adding test app ${test_app}")
        __add_test_app(${test_app} ${test_app}_interface)
    endforeach()

    # Print a list of created flash targets
    list(TRANSFORM test_apps APPEND "-flash" OUTPUT_VARIABLE test_apps_flash_targets)
    list(JOIN test_apps_flash_targets ", " test_apps_flash_targets_list_str)
    message("Created flash targets: flash (main app), ${test_apps_flash_targets_list_str}")
endfunction()
