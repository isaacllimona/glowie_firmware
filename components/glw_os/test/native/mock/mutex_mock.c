#include <stdbool.h>

#include <fff.h>

#include "glw_os/mutex.h"

DEFINE_FAKE_VALUE_FUNC0(os_mutex_handle_t, os_mutex_create);
DEFINE_FAKE_VALUE_FUNC2(bool, os_mutex_take, os_mutex_handle_t, os_time_t);
DEFINE_FAKE_VALUE_FUNC1(bool, os_mutex_give, os_mutex_handle_t);
