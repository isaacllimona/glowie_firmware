#pragma once

#include <stdint.h>
#include <fff.h>

#include "glw_imu/errors.h"

DECLARE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_init);
DECLARE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_write, uint8_t, const uint8_t*, uint16_t);
DECLARE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_read, uint8_t, uint8_t*, uint16_t);

inline static void reset_all_fakes(void) {
    RESET_FAKE(glw_imu_bus_init);
    RESET_FAKE(glw_imu_bus_write);
    RESET_FAKE(glw_imu_bus_read);
}
