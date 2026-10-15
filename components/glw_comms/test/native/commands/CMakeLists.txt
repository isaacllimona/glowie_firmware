add_executable(commands_test_app
    test_commands.c
)

target_link_libraries(commands_test_app
  native_test_common
  glw_comms_commands
)

add_test(NAME commands_test COMMAND commands_test_app)
