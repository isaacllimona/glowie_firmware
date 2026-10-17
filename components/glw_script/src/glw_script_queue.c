#include <assert.h>
#include <string.h>

#include "glw_os/queue.h"
#include "glw_os/shared_buffers.h"

#include "config.h"
#include "log.h"

// implements
#include "glw_script.h"
#include "glw_script_queue.h"

static const int queue_size = CONFIG_SCRIPT_QUEUE_SIZE;

static os_queue_handle_t script_queue;

// Internal buffer pool to keep scripts given as a string
static char buffer_pool_mem[SHARED_BUFFER_POOL_REQUIRED_MEMORY(CONFIG_SCRIPT_BUFFER_POOL_BUFFER_SIZE,
                                                               CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS)];
static shared_buffer_pool_handle buffer_pool;

// ---- glw_script_queue.h

void glw_script_queue_init(void) {
    script_queue = os_queue_create(queue_size, sizeof(shared_buffer_handle));
    assert(script_queue != NULL);

    buffer_pool = shared_buffer_pool_create(buffer_pool_mem, CONFIG_SCRIPT_BUFFER_POOL_BUFFER_SIZE,
                                            CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS);
    assert(buffer_pool != NULL);
}

shared_buffer_handle glw_script_queue_wait(unsigned int timeout) {
    shared_buffer_handle buffer;
    int ret = os_queue_receive(script_queue, &buffer, timeout);
    if (ret != OS_QUEUE_OK) return NULL;
    else return buffer;
}

// ---- glw_script.h (public interface)

glw_script_err_t glw_script_run_string(const char *script_str) {
    shared_buffer_handle buffer = shared_buffer_alloc_from_string(buffer_pool, script_str);
    if (buffer == NULL) {
        if (strlen(script_str) > CONFIG_SCRIPT_BUFFER_POOL_BUFFER_SIZE * CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS)
            return GLW_SCRIPT_ERR_SCRIPT_TOO_LONG;
        else
            return GLW_SCRIPT_ERR_BUFFERS_BUSY;
    }

    glw_script_err_t ret = glw_script_run_buffer(buffer);
    shared_buffer_release(buffer);
    return ret;
}

glw_script_err_t glw_script_run_buffer(shared_buffer_handle buffer) {
    shared_buffer_claim(buffer);

    os_queue_err_t ret = os_queue_send(script_queue, &buffer, 0);

    if (ret != OS_QUEUE_OK)
        shared_buffer_release(buffer);

    switch (ret) {
        case OS_QUEUE_OK:
            return GLW_SCRIPT_OK;
        case OS_QUEUE_ERR_QUEUE_FULL:
            return GLW_SCRIPT_ERR_QUEUE_FULL;
        default:
            LOGE("Unexpected error from os_queue_send: %d", ret);
            return GLW_SCRIPT_ERR_OTHER;
    }
}
