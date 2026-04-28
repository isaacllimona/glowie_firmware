#pragma once

#include <stdint.h>

typedef int16_t glw_inrt_imu_acc_t, glw_inrt_imu_gyro_t;
typedef unsigned int glw_inrt_imu_time_t;

/**
 * IMU-specific constants
 */
extern const struct glw_inrt_imu_consts{
    glw_inrt_imu_acc_t acc_max;
    glw_inrt_imu_gyro_t gyro_max;
} glw_inrt_imu_consts;

/**
 * Structure to contain the raw sensor data
 */
typedef struct {
    enum : uint8_t {
        GLW_INRT_DATA_CONTAINS_GYRO = 0b01,
        GLW_INRT_DATA_CONTAINS_ACC = 0b10,
    } flags;

    glw_inrt_imu_gyro_t gyro[3];
    glw_inrt_imu_acc_t acc[3];
    glw_inrt_imu_time_t timestamp;
} glw_inrt_imu_data_t;
