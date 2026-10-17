#pragma once

#include "glw_os/mutex.h"

#define SHARED_BUFFER_POOL_REQUIRED_MEMORY(buffer_size, num_buffers) (sizeof(struct shared_buffer_pool) + num_buffers * (sizeof(struct shared_buffer) + buffer_size))

typedef struct shared_buffer *shared_buffer_handle;
typedef struct shared_buffer_pool *shared_buffer_pool_handle;

struct shared_buffer {
    char * const data;
    unsigned int len;
    shared_buffer_handle next;
    int owner_count;
    const shared_buffer_pool_handle pool;
};

struct shared_buffer_pool {
    shared_buffer_handle head;
    const unsigned int buffer_size;
    os_mutex_handle_t mutex;
};

typedef struct {
    shared_buffer_handle buffer_cursor;
} shared_buffer_reader;

shared_buffer_pool_handle shared_buffer_pool_create(char *memory, unsigned int buffer_size, unsigned int num_buffers);

shared_buffer_handle shared_buffer_alloc(shared_buffer_pool_handle pool);

shared_buffer_handle shared_buffer_alloc_size(shared_buffer_pool_handle pool, unsigned int size);

void shared_buffer_free(shared_buffer_handle buffer);

void shared_buffer_claim(shared_buffer_handle buffer);

void shared_buffer_release(shared_buffer_handle buffer);

int shared_buffer_append_string(shared_buffer_handle buffer, const char *str);

shared_buffer_handle shared_buffer_alloc_from_string(shared_buffer_pool_handle pool, const char *str);

shared_buffer_reader shared_buffer_reader_create(shared_buffer_handle buffer);

const char *shared_buffer_reader_read(shared_buffer_reader *reader, unsigned int *length);
