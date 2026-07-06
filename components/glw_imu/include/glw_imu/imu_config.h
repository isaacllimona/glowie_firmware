#pragma once

#include <stdint.h>

/**
 * Output data rate (ODR).
 */
typedef enum {
    GLW_IMU_ODR_OFF = 0,
    GLW_IMU_ODR_12Hz5,
    GLW_IMU_ODR_25Hz,
    GLW_IMU_ODR_50Hz,
    GLW_IMU_ODR_100Hz,
    GLW_IMU_ODR_200Hz,
    GLW_IMU_ODR_400Hz,
    GLW_IMU_ODR_800Hz,
    GLW_IMU_ODR_1600Hz,
    GLW_IMU_ODR_3200Hz,
    GLW_IMU_ODR_6400Hz,
} glw_imu_odr_t;

/**
 * Accelerometer full-scale range.
 */
typedef enum {
    GLW_IMU_ACCEL_FS_4G,
    GLW_IMU_ACCEL_FS_8G,
    GLW_IMU_ACCEL_FS_16G,
    GLW_IMU_ACCEL_FS_32G,
} glw_imu_accel_fs_t;

/**
 * Gyroscope full-scale range.
 */
typedef enum {
    GLW_IMU_GYRO_FS_125DPS,
    GLW_IMU_GYRO_FS_250DPS,
    GLW_IMU_GYRO_FS_500DPS,
    GLW_IMU_GYRO_FS_1000DPS,
    GLW_IMU_GYRO_FS_2000DPS,
    GLW_IMU_GYRO_FS_4000DPS,
} glw_imu_gyro_fs_t;

/**
 * Accelerometer configuration.
 */
typedef struct {
    glw_imu_odr_t odr;
    glw_imu_accel_fs_t full_scale;
} glw_imu_accel_config_t;

/**
 * Gyroscope configuration.
 */
typedef struct {
    glw_imu_odr_t odr;
    glw_imu_gyro_fs_t full_scale;
} glw_imu_gyro_config_t;
