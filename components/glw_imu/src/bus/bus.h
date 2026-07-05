#pragma once

#include <stdint.h>

#include "glw_imu/errors.h"

glw_imu_error_t glw_imu_bus_init(void);

glw_imu_error_t glw_imu_bus_write(uint8_t addr, const uint8_t *buffer, uint16_t len);

glw_imu_error_t glw_imu_bus_read(uint8_t addr, uint8_t *buffer, uint16_t len);
