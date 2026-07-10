
#pragma once

#include <stdbool.h>

#include "glw_os/time.h"

typedef os_time_t glw_script_time_t;
static const glw_script_time_t max_timeout = os_max_timeout;

glw_script_time_t get_time_ms(void);

/**
 * wait_for_input: waits for script input.
 * Must be called from the script task created by start_task;
 *
 * Returns a pointer to the null-terminated script string if a script is received
 * If no script is received before the timeout, returns NULL.
 *
 * If a script is received, release_input() must be called after the script has been processed
 * release_input() must not be called if no script has been received
 */
char* wait_for_input(glw_script_time_t timeout);
void release_input(void);

typedef void (*task_fn_t)(void);

void script_task_start(task_fn_t task_fn);
void script_task_cancel(void);
