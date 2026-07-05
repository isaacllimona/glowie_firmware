add_library(native_test_common INTERFACE)

target_include_directories(native_test_common INTERFACE include)

target_link_libraries(native_test_common INTERFACE
    unity
    fff
)
