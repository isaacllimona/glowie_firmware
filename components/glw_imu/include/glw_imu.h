#pragma once

#include "glw_imu/errors.h"
#include "glw_imu/imu_config.h"

glw_imu_error_t glw_imu_init(void);

glw_imu_error_t glw_imu_configure_gyro(const glw_imu_gyro_config_t *config);
glw_imu_error_t glw_imu_configure_accel(const glw_imu_accel_config_t *config);
