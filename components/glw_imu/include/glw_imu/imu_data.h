#pragma once

#include <stdint.h>

/**
 * Structure to contain the formatted sensor data
 */
typedef struct {
    enum : uint8_t {
        GLW_IMU_DATA_CONTAINS_GYRO = 0b01,
        GLW_IMU_DATA_CONTAINS_ACC = 0b10,
    } flags;
    int16_t gyro[3];
    int16_t acc[3];
} glw_imu_data_frame_t;
