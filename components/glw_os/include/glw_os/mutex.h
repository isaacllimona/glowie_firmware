#pragma once

#include <stdbool.h>

#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"

typedef unsigned int os_time_t;
static const os_time_t os_max_timeout = 0xffffffffU;

typedef SemaphoreHandle_t os_mutex_handle_t;

static inline os_mutex_handle_t os_mutex_create(void) { return xSemaphoreCreateMutex(); }

static inline bool os_mutex_take(os_mutex_handle_t mutex, os_time_t timeout) {
    return xSemaphoreTake(mutex, timeout / portTICK_PERIOD_MS) == pdTRUE;
}

static inline bool os_mutex_give(os_mutex_handle_t mutex) {
    return xSemaphoreGive(mutex) == pdTRUE;
}
