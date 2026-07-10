#include <stdatomic.h>
#include <stdbool.h>

#include "config.h"
#include "glw_os/task.h"
#include "glw_os/time.h"
#include "log.h"

// implements
#include "glw_script.h"
#include "glw_script_task.h"

static os_task_handle_t script_task_handle;

static atomic_flag is_buffer_taken = ATOMIC_FLAG_INIT;

static char script_buffer[CONFIG_SCRIPT_INPUT_BUFFER_SIZE];
static char const *script_buffer_end = script_buffer + sizeof(script_buffer);
static char *script_buffer_cursor = script_buffer;

glw_script_time_t get_time_ms(void) { return os_time(); }

char *wait_for_input(glw_script_time_t timeout_ms) {
    if (!os_task_notify_take(timeout_ms))
        return NULL;

    return script_buffer;
}

void release_input(void) { atomic_flag_clear(&is_buffer_taken); }

int glw_script_run_string(const char *script_str) {
    if (atomic_flag_test_and_set(&is_buffer_taken))
        return 1;

    size_t script_len = strlen(script_str);
    bool script_continues = false;

    // Check if the script shall be continued in another command
    if (script_str[script_len - 1] == '\\') {
        script_len--; // we don't add the backslash to the script
        script_continues = true;
    } else
        script_len += 1; // add the termination character

    // Check for buffer overflow
    if (script_buffer_cursor + script_len > script_buffer_end) {
        LOGE("Script buffer full");

        // reset and release buffer
        script_buffer_cursor = script_buffer;
        atomic_flag_clear(&is_buffer_taken);

        return 1;
    }

    // Copy input to buffer
    memcpy(script_buffer_cursor, script_str, script_len);

    if (script_continues) {
        script_buffer_cursor += script_len;
        // free the buffer so the script can be continued
        atomic_flag_clear(&is_buffer_taken);
    } else {
        script_buffer_cursor = script_buffer; // reset buffer
        // Notify the script task that a script is available.
        // The buffer taken flag is kept set until the script is consumed
        os_task_notify(script_task_handle);
    }

    return 0;
}

static void script_task(void *param) {
    task_fn_t task_fn = (task_fn_t)param;
    task_fn();
}

void script_task_start(task_fn_t task_fn) {
    script_task_handle = os_task_create(script_task, "script_runner", CONFIG_SCRIPT_TASK_STACK_SIZE, task_fn,
                                        CONFIG_SCRIPT_TASK_PRIORITY);
}

void script_task_cancel() { os_task_delete(script_task_handle); }
