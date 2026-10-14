#include <stddef.h>
#include <string.h>

#include "glw_os/shared_buffers.h"

#define SET_CONST(type, var, value) (*((type*) &(var)) = (value))

shared_buffer_pool_handle shared_buffer_pool_create(char *memory, unsigned int buffer_size, unsigned int num_buffers) {
    shared_buffer_pool_handle pool = (shared_buffer_pool_handle) memory;
    memory += sizeof(*pool);

    SET_CONST(unsigned int, pool->buffer_size, buffer_size);
    pool->mutex = os_mutex_create();
    pool->head = NULL;

    for(int i = 0; i < num_buffers; i++) {
        // allocate item struct
        shared_buffer_handle new_item = (shared_buffer_handle) memory;
        memory += sizeof(struct shared_buffer);

        // allocate item data region
        char *new_buffer_data = memory;
        memory += buffer_size;

        // Set const members (underlying memory is not const so casting away constness should be safe)
        SET_CONST(char*, new_item->data, new_buffer_data);
        SET_CONST(shared_buffer_pool_handle, new_item->pool, pool);

        // add buffer to pool
        new_item->next = pool->head;
        pool->head = new_item;
    }

    return pool;
}

shared_buffer_handle shared_buffer_alloc(shared_buffer_pool_handle pool) {
    os_mutex_take(pool->mutex, os_max_timeout);
    shared_buffer_handle buffer = pool->head;
    if(buffer != NULL) pool->head = buffer->next;;
    os_mutex_give(pool->mutex);

    if(buffer != NULL) {
        buffer->size = 0;
        buffer->next = NULL;
        buffer->owner_count = 1;
    }
    return buffer;
}

shared_buffer_handle shared_buffer_alloc_size(shared_buffer_pool_handle pool, unsigned int size) {
    unsigned int num_buffers = (size + pool->buffer_size - 1) / pool->buffer_size;  // ceil division

    os_mutex_take(pool->mutex, os_max_timeout);
    shared_buffer_handle head = pool->head;
    shared_buffer_handle tail = head;
    for (int i = 1; i < num_buffers; i++) {
        tail = tail->next;
        if (tail == NULL) break;
    }
    if (tail != NULL) pool->head = tail->next;
    os_mutex_give(pool->mutex);

    if (tail == NULL) return NULL;  // not enough buffers available

    tail->next = NULL;
    for (shared_buffer_handle it = head; it != NULL; it = it->next) {
        it->size = 0;
        it->owner_count = 1;
    }

    return head;
}

void shared_buffer_free(shared_buffer_handle buffer) {
    shared_buffer_handle tail = buffer;
    while (tail->next != NULL) tail = tail->next;

    os_mutex_take(buffer->pool->mutex, os_max_timeout);
    tail->next = buffer->pool->head;
    buffer->pool->head = buffer;
    os_mutex_give(buffer->pool->mutex);
}

void shared_buffer_claim(shared_buffer_handle buffer) {
    os_mutex_take(buffer->pool->mutex, os_max_timeout);
    buffer->owner_count += 1;
    os_mutex_give(buffer->pool->mutex);
}

void shared_buffer_release(shared_buffer_handle buffer) {
    os_mutex_take(buffer->pool->mutex, os_max_timeout);
    if (buffer->owner_count > 0) {
        buffer->owner_count -= 1;
        if (buffer->owner_count == 0) {
            shared_buffer_free(buffer);
        }
    }
    os_mutex_give(buffer->pool->mutex);
}
