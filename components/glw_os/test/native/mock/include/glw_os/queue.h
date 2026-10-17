#pragma once

#include <fff.h>

#include "glw_os/common.h"

typedef enum {
    OS_QUEUE_OK = 0,
    OS_QUEUE_ERR_QUEUE_FULL,
    OS_QUEUE_ERR_QUEUE_EMPTY,
    OS_QUEUE_ERR_OTHER
} os_queue_err_t;

typedef void *os_queue_handle_t;

DECLARE_FAKE_VALUE_FUNC2(os_queue_handle_t, os_queue_create, unsigned int, unsigned int);
DECLARE_FAKE_VALUE_FUNC3(os_queue_err_t, os_queue_send, os_queue_handle_t, const void *, os_time_t);
DECLARE_FAKE_VALUE_FUNC3(os_queue_err_t, os_queue_receive, os_queue_handle_t, void *, os_time_t);

static inline void glw_os_queue_reset_mocks(void) {
    RESET_FAKE(os_queue_create);
    RESET_FAKE(os_queue_send);
    RESET_FAKE(os_queue_receive);
}
