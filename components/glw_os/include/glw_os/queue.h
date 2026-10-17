#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"

#include "common.h"

typedef enum {
    OS_QUEUE_OK = 0,
    OS_QUEUE_ERR_QUEUE_FULL,
    OS_QUEUE_ERR_QUEUE_EMPTY,
    OS_QUEUE_ERR_OTHER
} os_queue_err_t;

typedef QueueHandle_t os_queue_handle_t;

static inline os_queue_handle_t os_queue_create(unsigned int queue_length, unsigned int item_size) {
    return xQueueCreate(queue_length, item_size);
}

static inline void os_queue_delete(os_queue_handle_t queue) {
    vQueueDelete(queue);
}

static inline os_queue_err_t os_queue_send(os_queue_handle_t queue, const void *data, os_time_t timeout) {
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout);
    BaseType_t ret = xQueueSend(queue, data, timeout_ticks);
    if (ret == pdPASS) return OS_QUEUE_OK;
    else if (ret == errQUEUE_FULL) return OS_QUEUE_ERR_QUEUE_FULL;
    else return OS_QUEUE_ERR_OTHER;
}

static inline int os_queue_receive(os_queue_handle_t queue, void *data_buffer, os_time_t timeout) {
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout);
    BaseType_t ret = xQueueReceive(queue, data_buffer, timeout_ticks);
    if(ret == pdPASS) return OS_QUEUE_OK;
    else if (ret == errQUEUE_EMPTY) return OS_QUEUE_ERR_QUEUE_EMPTY;
    else return OS_QUEUE_ERR_OTHER;
}
