#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"

typedef TaskHandle_t os_task_handle_t;
typedef void (*task_func_t)(void*);

static inline os_task_handle_t os_task_create(task_func_t func, const char *name, unsigned int stack_depth, void *param, unsigned int priority) {
    TaskHandle_t handle;
    BaseType_t ret = xTaskCreate(func, name, stack_depth, param, priority, &handle);
    if (ret == pdPASS) return handle;
    else return NULL;
}
