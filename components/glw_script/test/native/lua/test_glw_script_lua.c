#include <string.h>

#include "native_test_common.h"

#include "glw_os/shared_buffers.h"

// Include the implementation directly so we can reach the static
// load_shared_buffer() function under test.
#include "glw_script_lua.c"

#define TEST_BUFFER_SIZE 64
#define TEST_NUM_BUFFERS 4

static char pool_mem[SHARED_BUFFER_POOL_REQUIRED_MEMORY(TEST_BUFFER_SIZE, TEST_NUM_BUFFERS)];
static shared_buffer_pool_handle pool;

// Longer than TEST_BUFFER_SIZE so the script spans several pool buffers.
static const char multi_link_script[] =
    "-- This leading comment is deliberately long so that the script\n"
    "-- does not fit in a single pool buffer and is split over multiple links.\n"
    "return 7";

void setUp(void) {
    glw_os_mutex_reset_mocks();
    FFF_RESET_HISTORY();

    os_mutex_create_fake.return_val = (os_mutex_handle_t) 1;
    os_mutex_take_fake.return_val = true;
    os_mutex_give_fake.return_val = true;

    pool = shared_buffer_pool_create(pool_mem, TEST_BUFFER_SIZE, TEST_NUM_BUFFERS);
}

void tearDown(void) {}

static lua_State *new_lua_state(void) {
    lua_State *state = luaL_newstate();
    if (state != NULL) luaL_openlibs(state);
    return state;
}

// ======= TESTS =======

TEST_CASE(load_shared_buffer_single_link) {
    lua_State *state = new_lua_state();
    TEST_ASSERT_NOT_NULL(state);

    shared_buffer_handle buffer = shared_buffer_alloc_from_string(pool, "return 1 + 1");
    TEST_ASSERT_NOT_NULL(buffer);
    TEST_ASSERT_NULL(buffer->next);

    TEST_ASSERT_EQUAL(LUA_OK, load_shared_buffer(state, buffer));
    TEST_ASSERT_EQUAL(LUA_OK, lua_pcall(state, 0, 1, 0));
    TEST_ASSERT_EQUAL(2, lua_tointeger(state, -1));

    lua_close(state);
}

TEST_CASE(load_shared_buffer_spans_multiple_links) {
    TEST_ASSERT_TRUE(strlen(multi_link_script) > TEST_BUFFER_SIZE);

    lua_State *state = new_lua_state();
    TEST_ASSERT_NOT_NULL(state);

    shared_buffer_handle buffer = shared_buffer_alloc_from_string(pool, multi_link_script);
    TEST_ASSERT_NOT_NULL(buffer);
    TEST_ASSERT_NOT_NULL(buffer->next);

    TEST_ASSERT_EQUAL(LUA_OK, load_shared_buffer(state, buffer));
    TEST_ASSERT_EQUAL(LUA_OK, lua_pcall(state, 0, 1, 0));
    TEST_ASSERT_EQUAL(7, lua_tointeger(state, -1));

    lua_close(state);
}

TEST_CASE(load_shared_buffer_reports_syntax_error) {
    lua_State *state = new_lua_state();
    TEST_ASSERT_NOT_NULL(state);

    shared_buffer_handle buffer = shared_buffer_alloc_from_string(pool, "this is not valid lua @@@");
    TEST_ASSERT_NOT_NULL(buffer);

    TEST_ASSERT_EQUAL(LUA_ERRSYNTAX, load_shared_buffer(state, buffer));

    lua_close(state);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_load_shared_buffer_single_link);
    RUN_TEST(test_load_shared_buffer_spans_multiple_links);
    RUN_TEST(test_load_shared_buffer_reports_syntax_error);

    return UNITY_END();
}
