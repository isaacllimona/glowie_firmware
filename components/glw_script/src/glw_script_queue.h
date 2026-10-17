#include "glw_os/shared_buffers.h"

void glw_script_queue_init(void);

shared_buffer_handle glw_script_queue_wait(unsigned int timeout);
