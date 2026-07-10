#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

static bool argparse__is_delimiter(char c) { return c == ' '; }

static bool argparse_check_cmd(char *input, char *command, char **args) {
    // compare given command with input
    while (*command != 0) {
        if (*input != *command)
            return false;
        input++;
        command++;
    }

    // command must be followed by string termination or whitespace
    if (input[0] != 0 && !argparse__is_delimiter(input[0]))
        return false;

    // go to the beginning of the arguments
    if (input[0] != 0)
        input++;
    *args = input;

    return true;
}

static bool argparse_parse_ulong(char *input, unsigned long *value, char **next) {
    // strtoul does not set an error if the input is negative. Check manually
    if (*input == '-')
        return false;

    char *end;
    errno = 0;
    *value = strtoul(input, &end, 10);

    if (errno == 0 && end != input && (*end == 0 || argparse__is_delimiter(*end))) {
        // Valid integer. Find start of next argument
        while (argparse__is_delimiter(*end))
            end++;
        *next = end;
        return true;
    } else {
        // Invalid argument
        return false;
    }
}
