#include "glw_os/queue.h"

DEFINE_FAKE_VALUE_FUNC2(os_queue_handle_t, os_queue_create, unsigned int, unsigned int);
DEFINE_FAKE_VALUE_FUNC3(os_queue_err_t, os_queue_send, os_queue_handle_t, const void *, os_time_t);
DEFINE_FAKE_VALUE_FUNC3(os_queue_err_t, os_queue_receive, os_queue_handle_t, void *, os_time_t);
