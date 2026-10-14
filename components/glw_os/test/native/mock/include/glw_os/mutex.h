#pragma once

#include <stdbool.h>

#include <fff.h>

#include "glw_os/common.h"

typedef void* os_mutex_handle_t;

DECLARE_FAKE_VALUE_FUNC0(os_mutex_handle_t, os_mutex_create);
DECLARE_FAKE_VALUE_FUNC2(bool, os_mutex_take, os_mutex_handle_t, os_time_t);
DECLARE_FAKE_VALUE_FUNC1(bool, os_mutex_give, os_mutex_handle_t);

static inline void glw_os_mutex_reset_mocks(void) {
    RESET_FAKE(os_mutex_create);
    RESET_FAKE(os_mutex_take);
    RESET_FAKE(os_mutex_give);
}
