#include <string.h>

#include "native_test_common.h"

#include "glw_os/shared_buffers.h"

#define TEST_BUFFER_SIZE  64
#define TEST_POOL_MEM_SIZE 1024

static char pool_mem[TEST_POOL_MEM_SIZE];

static shared_buffer_pool_handle make_pool(unsigned int num_buffers) {
    return shared_buffer_pool_create(pool_mem, TEST_BUFFER_SIZE, num_buffers);
}

static void assert_valid_buffer_in_pool(shared_buffer_handle buffer) {
    TEST_ASSERT_NOT_NULL(buffer);
    TEST_ASSERT_TRUE((char *) buffer >= pool_mem);
    TEST_ASSERT_TRUE((char *) buffer < pool_mem + TEST_POOL_MEM_SIZE);
}

void setUp(void) {
    RESET_FAKE(os_mutex_create);
    RESET_FAKE(os_mutex_take);
    RESET_FAKE(os_mutex_give);
    FFF_RESET_HISTORY();

    os_mutex_take_fake.return_val = true;
    os_mutex_give_fake.return_val = true;
}

void tearDown(void) {}

// ======= TESTS =======
// --- pool creation

TEST_CASE(pool_create_builds_free_list) {
    unsigned int num_buffers = 4;

    shared_buffer_pool_handle pool = make_pool(num_buffers);

    TEST_ASSERT_EQUAL_PTR(pool_mem, pool);
    TEST_ASSERT_EQUAL(TEST_BUFFER_SIZE, pool->buffer_size);

    for (int i = 0; i < num_buffers; i++) {
        shared_buffer_handle buffer = shared_buffer_alloc(pool);
        assert_valid_buffer_in_pool(buffer);
    }
    TEST_ASSERT_NULL(shared_buffer_alloc(pool));
}

// --- allocation

TEST_CASE(alloc_returns_initialized_buffer) {
    shared_buffer_pool_handle pool = make_pool(4);

    shared_buffer_handle buffer = shared_buffer_alloc(pool);

    assert_valid_buffer_in_pool(buffer);
    TEST_ASSERT_EQUAL_PTR(pool, buffer->pool);
    TEST_ASSERT_EQUAL(0, buffer->size);
    TEST_ASSERT_EQUAL(1, buffer->owner_count);
    TEST_ASSERT_NULL(buffer->next);
    TEST_ASSERT_TRUE((char *) buffer->data >= pool_mem);
    TEST_ASSERT_TRUE((char *) buffer->data < pool_mem + TEST_POOL_MEM_SIZE);
}

TEST_CASE(alloc_exhausts_pool) {
    unsigned int num_buffers = 4;
    shared_buffer_pool_handle pool = make_pool(num_buffers);

    for (int i = 0; i < num_buffers; i++) {
        TEST_ASSERT_NOT_NULL(shared_buffer_alloc(pool));
    }
    TEST_ASSERT_NULL(shared_buffer_alloc(pool));
}

TEST_CASE(alloc_uses_mutex) {
    shared_buffer_pool_handle pool = make_pool(4);

    shared_buffer_alloc(pool);

    TEST_ASSERT_EQUAL(1, os_mutex_take_fake.call_count);
    TEST_ASSERT_EQUAL(1, os_mutex_give_fake.call_count);
}

// --- freeing

TEST_CASE(free_returns_buffer_to_pool) {
    shared_buffer_pool_handle pool = make_pool(2);

    shared_buffer_handle a = shared_buffer_alloc(pool);
    shared_buffer_handle b = shared_buffer_alloc(pool);

    shared_buffer_free(a);
    TEST_ASSERT_EQUAL_PTR(a, shared_buffer_alloc(pool));

    shared_buffer_free(b);
    TEST_ASSERT_EQUAL_PTR(b, shared_buffer_alloc(pool));
    TEST_ASSERT_NULL(shared_buffer_alloc(pool));
}

TEST_CASE(free_returns_buffer_chain_to_pool) {
    shared_buffer_pool_handle pool = make_pool(4);

    // build a 2-buffer chain, as alloc_size would
    shared_buffer_handle b1 = shared_buffer_alloc(pool);
    shared_buffer_handle b2 = shared_buffer_alloc(pool);
    b1->next = b2;

    shared_buffer_free(b1);

    // freeing the chain head must return every buffer in the chain to the pool
    TEST_ASSERT_EQUAL_PTR(b1, shared_buffer_alloc(pool));
    TEST_ASSERT_EQUAL_PTR(b2, shared_buffer_alloc(pool));
    TEST_ASSERT_NOT_NULL(shared_buffer_alloc(pool));
    TEST_ASSERT_NOT_NULL(shared_buffer_alloc(pool));
    TEST_ASSERT_NULL(shared_buffer_alloc(pool));
}

// --- ownership

TEST_CASE(claim_increments_owner_count) {
    shared_buffer_pool_handle pool = make_pool(1);

    shared_buffer_handle buffer = shared_buffer_alloc(pool);
    TEST_ASSERT_EQUAL(1, buffer->owner_count);

    shared_buffer_claim(buffer);
    TEST_ASSERT_EQUAL(2, buffer->owner_count);

    // buffer is still referenced, so it must not be returned to the pool
    shared_buffer_release(buffer);
    TEST_ASSERT_EQUAL(1, buffer->owner_count);
    TEST_ASSERT_NULL(shared_buffer_alloc(pool));
}

TEST_CASE(release_last_owner_frees_buffer) {
    shared_buffer_pool_handle pool = make_pool(1);

    shared_buffer_handle buffer = shared_buffer_alloc(pool);

    shared_buffer_release(buffer);

    TEST_ASSERT_EQUAL_PTR(buffer, shared_buffer_alloc(pool));
}

TEST_CASE(release_path_uses_mutex_balanced) {
    shared_buffer_pool_handle pool = make_pool(1);

    shared_buffer_handle buffer = shared_buffer_alloc(pool);
    shared_buffer_claim(buffer);
    shared_buffer_release(buffer);

    // alloc, claim and release each take/give once; the buffer is still
    // referenced (owner count 2 -> 1), so no free happens
    TEST_ASSERT_EQUAL(3, os_mutex_take_fake.call_count);
    TEST_ASSERT_EQUAL(3, os_mutex_give_fake.call_count);
}

// --- alloc_size

TEST_CASE(alloc_size_insufficient_returns_null) {
    shared_buffer_pool_handle pool = make_pool(1);

    shared_buffer_handle buffer = shared_buffer_alloc_size(pool, 2 * TEST_BUFFER_SIZE);

    TEST_ASSERT_NULL(buffer);
}

TEST_CASE(alloc_size_single_buffer_fit) {
    shared_buffer_pool_handle pool = make_pool(4);

    shared_buffer_handle buffer = shared_buffer_alloc_size(pool, TEST_BUFFER_SIZE);

    assert_valid_buffer_in_pool(buffer);
    TEST_ASSERT_EQUAL(0, buffer->size);
    TEST_ASSERT_NULL(buffer->next);
}

TEST_CASE(alloc_size_spans_multiple_buffers) {
    shared_buffer_pool_handle pool = make_pool(4);

    shared_buffer_handle buffer = shared_buffer_alloc_size(pool, 2 * TEST_BUFFER_SIZE);

    assert_valid_buffer_in_pool(buffer);
    TEST_ASSERT_NOT_NULL(buffer->next);
    assert_valid_buffer_in_pool(buffer->next);
    TEST_ASSERT_NULL(buffer->next->next);
}

TEST_CASE(alloc_size_fills_buffer_partially) {
    shared_buffer_pool_handle pool = make_pool(4);

    shared_buffer_handle buffer = shared_buffer_alloc_size(pool, TEST_BUFFER_SIZE + 1);

    assert_valid_buffer_in_pool(buffer);
    TEST_ASSERT_NOT_NULL(buffer->next);
    assert_valid_buffer_in_pool(buffer->next);
    TEST_ASSERT_NULL(buffer->next->next);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pool_create_builds_free_list);
    RUN_TEST(test_alloc_returns_initialized_buffer);
    RUN_TEST(test_alloc_exhausts_pool);
    RUN_TEST(test_alloc_uses_mutex);

    RUN_TEST(test_free_returns_buffer_to_pool);
    RUN_TEST(test_free_returns_buffer_chain_to_pool);

    RUN_TEST(test_claim_increments_owner_count);
    RUN_TEST(test_release_last_owner_frees_buffer);
    RUN_TEST(test_release_path_uses_mutex_balanced);

    RUN_TEST(test_alloc_size_insufficient_returns_null);
    RUN_TEST(test_alloc_size_single_buffer_fit);
    RUN_TEST(test_alloc_size_spans_multiple_buffers);
    RUN_TEST(test_alloc_size_fills_buffer_partially);

    return UNITY_END();
}
