#include "glw_os/task.h"

#include "config.h"
#include "glw_script_queue.h"
#include "glw_script_lua.h"

// implements
#include "glw_script.h"

static os_task_handle_t script_task_handle;

static void script_task(void *param) {
    glw_script_lua_run();
}

void glw_script_init(const glw_script_init_t *init) {
    glw_script_queue_init();

    glw_script_lua_init_t lua_init = {
        .error_cb = init->error_cb,
        .get_sync_time = init->get_sync_time,
    };
    glw_script_lua_init(&lua_init);
}

void glw_script_start(void) {
    script_task_handle = os_task_create(script_task, "script_runner", CONFIG_SCRIPT_TASK_STACK_SIZE, NULL,
                                        CONFIG_SCRIPT_TASK_PRIORITY);
    assert(script_task_handle != NULL);
}

void glw_script_stop(void) {
    os_task_delete(script_task_handle);
}
