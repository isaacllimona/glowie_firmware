#include <unity.h>

#include "glw_imu.h"
#include "glw_imu/errors.h"

static void test_glw_imu_init(void) {
    glw_imu_error_t ret = glw_imu_init();
    TEST_ASSERT_EQUAL(0, ret);
}

void glw_imu_test_run_all() {
    RUN_TEST(test_glw_imu_init);
}
