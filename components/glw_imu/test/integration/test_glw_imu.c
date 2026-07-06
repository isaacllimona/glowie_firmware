#include <unity.h>

#include "glw_imu/imu_config.h"
#include "glw_os/time.h"

#include "glw_imu.h"
#include "glw_imu/fifo.h"
#include "glw_imu/imu_data.h"

#define ASSERT_OK(x) TEST_ASSERT_EQUAL(0, (x))

static char msg_buffer[100];
#define MESSAGE(msg, ...)                                                                                    \
    sprintf(msg_buffer, msg, ##__VA_ARGS__);                                                                 \
    TEST_MESSAGE(msg_buffer)

static void test_glw_imu_init(void) { ASSERT_OK(glw_imu_init()); }

static void test_glw_imu_fifo_acc_and_gyr(void) {
    glw_imu_accel_config_t acc_config = {.full_scale = GLW_IMU_ACCEL_FS_32G, .odr = GLW_IMU_ODR_3840Hz};
    glw_imu_gyro_config_t gyro_config = {.full_scale = GLW_IMU_GYRO_FS_4000DPS, .odr = GLW_IMU_ODR_3840Hz};
    ASSERT_OK(glw_imu_configure_accel(&acc_config));
    ASSERT_OK(glw_imu_configure_gyro(&gyro_config));

    ASSERT_OK(glw_imu_fifo_init());

    glw_imu_fifo_fetch_result_t fetch_result;

    glw_imu_fifo_fetch(NULL);
    os_delay(10);
    ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    MESSAGE("fetched %u/%u entries", fetch_result.fetched_entries, fetch_result.available_entries);

    int frame_cnt = 0;
    glw_imu_data_frame_t frame;

    while (1) {
        glw_imu_fifo_read_frame(&frame);
        if (frame.flags == 0)
            break;

        frame_cnt++;
        TEST_ASSERT_EQUAL(GLW_IMU_DATA_CONTAINS_ACC | GLW_IMU_DATA_CONTAINS_GYRO, frame.flags);
    }

    TEST_ASSERT_EQUAL(fetch_result.fetched_entries,
                      frame_cnt * 2); // We should get a frame for every 2 entries
}

static void test_glw_imu_fifo_fetch_empty(void) {
    glw_imu_accel_config_t acc_config = {.full_scale = GLW_IMU_ACCEL_FS_32G, .odr = GLW_IMU_ODR_1Hz875};
    glw_imu_gyro_config_t gyro_config = {.full_scale = GLW_IMU_GYRO_FS_4000DPS, .odr = GLW_IMU_ODR_1Hz875};
    ASSERT_OK(glw_imu_configure_accel(&acc_config));
    ASSERT_OK(glw_imu_configure_gyro(&gyro_config));

    ASSERT_OK(glw_imu_fifo_init());

    glw_imu_fifo_fetch_result_t fetch_result;

    ASSERT_OK(glw_imu_fifo_fetch(NULL));
    ASSERT_OK(glw_imu_fifo_fetch(NULL));
    ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(0, fetch_result.fetched_entries);
}

void glw_imu_test_run_all() {
    RUN_TEST(test_glw_imu_init);
    RUN_TEST(test_glw_imu_fifo_acc_and_gyr);
    RUN_TEST(test_glw_imu_fifo_fetch_empty);
}
