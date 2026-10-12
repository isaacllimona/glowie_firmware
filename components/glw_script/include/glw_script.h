#pragma once

#include <stdint.h>
#include <strings.h>

typedef struct {
    unsigned int (*get_time)(void);
    void (*error_cb)(const char* msg);
} glw_script_init_t;

/** Configure the glw_script module */
void glw_script_init(const glw_script_init_t *init);

/** Start the interpreter and wait for input */
void glw_script_start(void);

/** Stop the interpreter */
void glw_script_stop(void);

/** Queue a script for running */
int glw_script_run_string(const char * script_str);
