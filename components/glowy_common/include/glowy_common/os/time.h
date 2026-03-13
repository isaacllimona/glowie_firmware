#pragma once

#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "portmacro.h"

// ---- delays

static inline void os_delay(unsigned int time_ms) { vTaskDelay(time_ms / portTICK_PERIOD_MS); }

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
