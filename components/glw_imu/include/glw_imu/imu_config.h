#pragma once

#include <stdint.h>

/**
 * Output data rate (ODR).
 */
typedef enum {
    GLW_IMU_ODR_OFF = 0,
    GLW_IMU_ODR_1Hz875,
    GLW_IMU_ODR_7Hz5,
    GLW_IMU_ODR_15Hz,
    GLW_IMU_ODR_30Hz,
    GLW_IMU_ODR_60Hz,
    GLW_IMU_ODR_120Hz,
    GLW_IMU_ODR_240Hz,
    GLW_IMU_ODR_480Hz,
    GLW_IMU_ODR_960Hz,
    GLW_IMU_ODR_1920Hz,
    GLW_IMU_ODR_3840Hz,
    GLW_IMU_ODR_7680Hz,
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
