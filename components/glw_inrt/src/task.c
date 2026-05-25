#include "glw_os/task.h"

#include "glw_inrt/glw_inrt.h"

// implements
#include "glw_inrt/task.h"
#include <stdbool.h>

const unsigned int stack_depth = 10000;
const char *task_name = "glw_inrt";

static void task_func(void *params) {
    glw_inrt_run();
}

bool glw_inrt_task_start(unsigned int priority) {
    os_task_handle_t task = os_task_create(task_func, task_name, stack_depth, NULL, priority);
    return task != NULL;
}
