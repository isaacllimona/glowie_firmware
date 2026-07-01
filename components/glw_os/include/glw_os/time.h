#pragma once

#include "assert.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"

typedef unsigned long os_time_t;

// ---- get time

// Get a time in milliseconds
static inline os_time_t os_time(void) {
    return pdTICKS_TO_MS(xTaskGetTickCount());
}

// ---- delays

static inline void os_delay(os_time_t time_ms) { vTaskDelay(time_ms / portTICK_PERIOD_MS); }

// ---- timer

typedef struct {
    TickType_t target_period;
    TickType_t lastWakeTime;
} os_timer;

static inline void os_timer_init(os_timer *timer, unsigned int target_frequency_hz) {
    timer->target_period = pdMS_TO_TICKS(1000 / target_frequency_hz);
    timer->lastWakeTime = xTaskGetTickCount();
}

static inline void os_timer_sleep(os_timer *timer) {
    vTaskDelayUntil(&timer->lastWakeTime, timer->target_period);
}
