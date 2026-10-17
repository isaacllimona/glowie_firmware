#pragma once

#include <stdint.h>
#include <strings.h>

#include "glw_os/shared_buffers.h"

typedef enum {
    GLW_SCRIPT_OK = 0,
    GLW_SCRIPT_ERR_QUEUE_FULL,  // The script queue is full
    GLW_SCRIPT_ERR_BUFFERS_BUSY,  // Not enough space available in the internal buffers
    GLW_SCRIPT_ERR_SCRIPT_TOO_LONG,  // The script is too long to fit in the internal script buffers
    GLW_SCRIPT_ERR_OTHER,
} glw_script_err_t;

typedef struct {
    unsigned int (*get_sync_time)(void);
    void (*error_cb)(const char* msg);
} glw_script_init_t;

/** Configure the glw_script module */
void glw_script_init(const glw_script_init_t *init);

/** Start the interpreter and wait for input */
void glw_script_start(void);

/** Stop the interpreter */
void glw_script_stop(void);

/**
 * Queue a script for running
 *
 * Returns:
 * - GLW_SCRIPT_OK
 *      The script was successfully queued
 * - GLW_SCRIPT_ERR_QUEUE_FULL
 *      The queue is full
 * - GLW_SCRIPT_ERR_BUFFERS_BUSY
 *      There is not enough space available at the moment in the internal buffers,
 *      but there will be after pending scripts are processed
 * - GLW_SCRIPT_ERR_SCRIPT_TOO_LONG
 *      The script does not fit in the internal buffers
 * - GLW_SCRIPT_ERR_OTHER
 *      Other errors
 */
glw_script_err_t glw_script_run_string(const char *script_str);

/**
 * Queue a script for running
 *
 * Returns:
 * - GLW_SCRIPT_OK
 *      The script was successfully queued
 * - GLW_SCRIPT_ERR_QUEUE_FULL
 *      The queue is full
 * - GLW_SCRIPT_ERR_OTHER
 *      Other errors
 */
glw_script_err_t glw_script_run_buffer(shared_buffer_handle buffer);
