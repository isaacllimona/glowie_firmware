add_executable(commands_test_app
    test_commands.c
)

target_link_libraries(commands_test_app
    Unity
    fff
    glw_commands
)

add_test(NAME commands_test COMMAND commands_test_app)
