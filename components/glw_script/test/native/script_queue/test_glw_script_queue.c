#include <string.h>

#include "native_test_common.h"

#include "glw_os/queue.h"
#include "glw_os/shared_buffers.h"

#include "config.h"
#include "glw_script.h"
#include "glw_script_queue.h"

// Emulated queue content. os_queue_send()/os_queue_receive() copy one
// shared_buffer_handle
static shared_buffer_handle queued_item;

static os_queue_err_t queue_send_custom(os_queue_handle_t queue, const void *data, os_time_t timeout) {
    (void) queue;
    (void) timeout;

    memcpy(&queued_item, data, sizeof(queued_item));
    return OS_QUEUE_OK;
}

static os_queue_err_t queue_receive_custom(os_queue_handle_t queue, void *data_buffer, os_time_t timeout) {
    (void) queue;
    (void) timeout;

    memcpy(data_buffer, &queued_item, sizeof(queued_item));
    return OS_QUEUE_OK;
}

void setUp(void) {
    glw_os_mutex_reset_mocks();
    glw_os_queue_reset_mocks();
    FFF_RESET_HISTORY();

    os_mutex_create_fake.return_val = (os_mutex_handle_t) 1;
    os_mutex_take_fake.return_val = true;
    os_mutex_give_fake.return_val = true;

    os_queue_create_fake.return_val = (os_queue_handle_t) 1;
    os_queue_send_fake.custom_fake = queue_send_custom;
    os_queue_receive_fake.custom_fake = queue_receive_custom;

    queued_item = NULL;

    glw_script_queue_init();
}

void tearDown(void) {}

static shared_buffer_handle get_buffer(void) {
    // borrow a buffer from the internal pool
    glw_script_run_string("whatever");
    return glw_script_queue_wait(0);
}

// ======= TESTS =======

// --- run then wait

TEST_CASE(run_string_then_receive) {
    const char *test_string = "print('hello')";
    const unsigned int test_string_len = strlen(test_string);

    glw_script_err_t ret = glw_script_run_string(test_string);
    TEST_ASSERT_EQUAL(GLW_SCRIPT_OK, ret);

    shared_buffer_handle received = glw_script_queue_wait(0);

    TEST_ASSERT_EQUAL(test_string_len, received->len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(test_string, received->data, test_string_len);
}

TEST_CASE(run_buffer_then_receive) {
    shared_buffer_handle buffer = get_buffer();

    glw_script_err_t ret = glw_script_run_buffer(buffer);
    TEST_ASSERT_EQUAL(GLW_SCRIPT_OK, ret);

    shared_buffer_handle received = glw_script_queue_wait(0);

    TEST_ASSERT_EQUAL_PTR(buffer, received);
}

// --- buffer ownership

TEST_CASE(run_string_leaves_buffer_owned_by_queue) {
    glw_script_run_string("print('hello')");

    // single owner while in the queue
    TEST_ASSERT_EQUAL(1, queued_item->owner_count);

    // still one owner after receive
    shared_buffer_handle received = glw_script_queue_wait(0);
    TEST_ASSERT_EQUAL(1, received->owner_count);
}

TEST_CASE(run_buffer_claims_extra_ownership) {
    shared_buffer_handle buffer = get_buffer();  // buffer will have owner count = 1

    glw_script_run_buffer(buffer);

    // queue reference + the caller's reference
    TEST_ASSERT_EQUAL(2, buffer->owner_count);

    // same after receiving
    shared_buffer_handle received = glw_script_queue_wait(0);
    TEST_ASSERT_EQUAL(2, received->owner_count);
}

// --- errors

TEST_CASE(run_string_barely_fits) {
    static char long_string[CONFIG_SCRIPT_BUFFER_POOL_BUFFER_SIZE * CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS + 1];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = 0;

    TEST_ASSERT_EQUAL(GLW_SCRIPT_OK, glw_script_run_string(long_string));
}

TEST_CASE(run_string_rejects_too_long_script) {
    static char long_string[CONFIG_SCRIPT_BUFFER_POOL_BUFFER_SIZE * CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS + 2];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = 0;

    TEST_ASSERT_EQUAL(GLW_SCRIPT_ERR_SCRIPT_TOO_LONG, glw_script_run_string(long_string));
}

TEST_CASE(run_string_reports_buffers_busy_when_pool_exhausted) {
    // exhaust internal pool
    for (int i = 0; i < CONFIG_SCRIPT_BUFFER_POOL_NUM_BUFFERS; i++) {
        glw_script_run_string("something");
    }

    TEST_ASSERT_EQUAL(GLW_SCRIPT_ERR_BUFFERS_BUSY, glw_script_run_string("a"));
}

TEST_CASE(run_string_full_queue) {
    os_queue_send_fake.custom_fake = NULL;
    os_queue_send_fake.return_val = OS_QUEUE_ERR_QUEUE_FULL;

    TEST_ASSERT_EQUAL(GLW_SCRIPT_ERR_QUEUE_FULL, glw_script_run_string("a"));
}

TEST_CASE(run_buffer_full_queue) {
    shared_buffer_handle buffer = get_buffer();

    os_queue_send_fake.custom_fake = NULL;
    os_queue_send_fake.return_val = OS_QUEUE_ERR_QUEUE_FULL;

    TEST_ASSERT_EQUAL(GLW_SCRIPT_ERR_QUEUE_FULL, glw_script_run_buffer(buffer));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_run_string_then_receive);
    RUN_TEST(test_run_buffer_then_receive);

    RUN_TEST(test_run_string_leaves_buffer_owned_by_queue);
    RUN_TEST(test_run_buffer_claims_extra_ownership);

    RUN_TEST(test_run_string_barely_fits);
    RUN_TEST(test_run_string_rejects_too_long_script);
    RUN_TEST(test_run_string_reports_buffers_busy_when_pool_exhausted);
    RUN_TEST(test_run_string_full_queue);
    RUN_TEST(test_run_buffer_full_queue);

    return UNITY_END();
}
