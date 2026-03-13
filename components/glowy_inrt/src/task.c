#include "glowy_common/os/task.h"

#include "glowy_inrt/glowy_inrt.h"

// implements
#include "glowy_inrt/task.h"
#include <stdbool.h>

const unsigned int stack_depth = 10000;
const char *task_name = "glowy_inrt";

static void task_func(void *params) {
    glowy_inrt_run();
}

bool glowy_inrt_task_start(unsigned int priority) {
    os_task_handle_t task = os_task_create(task_func, task_name, stack_depth, NULL, priority);
    return task != NULL;
}
