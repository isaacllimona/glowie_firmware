#pragma once

#include <fff.h>

#include "glw_os/common.h"

typedef struct {
    unsigned int target_period;
    unsigned int lastWakeTime;
} os_timer;

DECLARE_FAKE_VALUE_FUNC0(os_time_t, os_time);
DECLARE_FAKE_VOID_FUNC1(os_delay, os_time_t);
DECLARE_FAKE_VOID_FUNC2(os_timer_init, os_timer *, unsigned int);
DECLARE_FAKE_VOID_FUNC1(os_timer_sleep, os_timer *);

static inline void glw_os_time_reset_mocks(void) {
    RESET_FAKE(os_time);
    RESET_FAKE(os_delay);
    RESET_FAKE(os_timer_init);
    RESET_FAKE(os_timer_sleep);
}
