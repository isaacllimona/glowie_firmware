#pragma once

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"

#include "common.h"

typedef TaskHandle_t os_task_handle_t;
typedef void (*task_func_t)(void*);

static inline os_task_handle_t os_task_create(task_func_t func, const char *name, unsigned int stack_depth, void *param, unsigned int priority) {
    TaskHandle_t handle;
    BaseType_t ret = xTaskCreate(func, name, stack_depth, param, priority, &handle);
    if (ret == pdPASS) return handle;
    else return NULL;
}

static inline void os_task_delete(os_task_handle_t task) {
    vTaskDelete(task);
}

/*
 * Send a direct notification to the task
 */
static inline void os_task_notify(os_task_handle_t task_handle) {
    // Note: xTaskNotifyGive always returns pdPASS. No need to check the return value
    xTaskNotifyGive(task_handle);
}

/*
 * Wait for a direct notification to the current task.
 * Returns true when a notification is received
 * Returns false if no notification is received before the timeout
 */
static inline bool os_task_notify_take(os_time_t timeout) {
    return ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeout));
}
