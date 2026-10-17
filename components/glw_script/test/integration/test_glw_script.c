#include <string.h>
#include <unity.h>

#include "glw_os/shared_buffers.h"
#include "glw_os/time.h"
#include "glw_script.h"
#include "glw_script_lua.h"
#include "lauxlib.h"
#include "lua.h"


#define ASSERT_OK(x) TEST_ASSERT_EQUAL(GLW_SCRIPT_OK, (x))
#define ASSERT_NOT_OK(x) TEST_ASSERT_NOT_EQUAL(GLW_SCRIPT_OK, (x))

static volatile unsigned int test_counter = 0;
static volatile bool error_occurred = false;
static char last_error_msg[256];

static char test_buffer_pool_mem[SHARED_BUFFER_POOL_REQUIRED_MEMORY(256, 2)];

static unsigned int get_sync_time_mock(void) {
    return os_time();
}

static void error_cb(const char *msg) {
    error_occurred = true;
    strncpy(last_error_msg, msg ? msg : "", sizeof(last_error_msg) - 1);
    last_error_msg[sizeof(last_error_msg) - 1] = '\0';
}

static int lua_test_increment(lua_State *L) {
    (void)L;
    test_counter++;
    return 0;
}

static const luaL_Reg test_lib_funcs[] = {
    {"increment", lua_test_increment},
    {NULL, NULL}
};

static void setup_glw_script() {
    glw_script_init_t init = {
        .get_sync_time = get_sync_time_mock,
        .error_cb = error_cb,
    };
    glw_script_init(&init);
    glw_script_lua_add_library(test_lib_funcs, "test");
    glw_script_start();
}

void setUp(void) {
    test_counter = 0;
    error_occurred = false;
    last_error_msg[0] = '\0';
}

static void test_run_script_as_string(void) {
    test_counter = 0;
    ASSERT_OK(glw_script_run_string("test.increment()"));
    os_delay(10);
    TEST_ASSERT_EQUAL(1, test_counter);
}

static void test_run_script_as_buffer(void) {
    shared_buffer_pool_handle pool = shared_buffer_pool_create(test_buffer_pool_mem, 256, 2);
    TEST_ASSERT_NOT_NULL(pool);

    test_counter = 0;
    shared_buffer_handle buf = shared_buffer_alloc_from_string(pool, "test.increment()");
    TEST_ASSERT_NOT_NULL(buf);

    ASSERT_OK(glw_script_run_buffer(buf));
    os_delay(10);
    TEST_ASSERT_EQUAL(1, test_counter);

    shared_buffer_release(buf);
}

static void test_buffers_get_released_properly(void) {
    shared_buffer_pool_handle pool = shared_buffer_pool_create(test_buffer_pool_mem, 256, 2);
    TEST_ASSERT_NOT_NULL(pool);

    test_counter = 0;
    for (int i = 0; i < 4; i++) {
        TEST_MESSAGE("increment");
        shared_buffer_handle buf = shared_buffer_alloc_from_string(pool, "test.increment()");
        TEST_ASSERT_NOT_NULL(buf);
        ASSERT_OK(glw_script_run_buffer(buf));
        shared_buffer_release(buf);
    }
    os_delay(20);
    TEST_ASSERT_EQUAL(4, test_counter);
}

void glw_script_test_run_all() {
    setup_glw_script();

    RUN_TEST(test_run_script_as_string);
    RUN_TEST(test_run_script_as_buffer);
    RUN_TEST(test_buffers_get_released_properly);
}
