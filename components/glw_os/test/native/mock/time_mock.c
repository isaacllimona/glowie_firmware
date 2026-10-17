#include "glw_os/time.h"

DEFINE_FAKE_VALUE_FUNC0(os_time_t, os_time);
DEFINE_FAKE_VOID_FUNC1(os_delay, os_time_t);
DEFINE_FAKE_VOID_FUNC2(os_timer_init, os_timer *, unsigned int);
DEFINE_FAKE_VOID_FUNC1(os_timer_sleep, os_timer *);
