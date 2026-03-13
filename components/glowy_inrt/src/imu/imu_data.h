#pragma once

#include <stdint.h>

typedef int16_t glowy_inrt_imu_acc_t, glowy_inrt_imu_gyro_t;
typedef unsigned int glowy_inrt_imu_time_t;

/**
 * IMU-specific constants
 */
extern const struct glowy_inrt_imu_consts{
    glowy_inrt_imu_acc_t acc_max;
    glowy_inrt_imu_gyro_t gyro_max;
} glowy_inrt_imu_consts;

/**
 * Structure to contain the raw sensor data
 */
typedef struct {
    enum : uint8_t {
        GLOWY_INRT_DATA_CONTAINS_GYRO = 0b01,
        GLOWY_INRT_DATA_CONTAINS_ACC = 0b10,
    } flags;

    glowy_inrt_imu_gyro_t gyro[3];
    glowy_inrt_imu_acc_t acc[3];
    glowy_inrt_imu_time_t timestamp;
} glowy_inrt_imu_data_t;
