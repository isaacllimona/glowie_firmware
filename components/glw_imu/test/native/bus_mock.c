#include <fff.h>

#include "glw_imu/errors.h"

#include "bus_mock.h"

DEFINE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_init);
DEFINE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_write, uint8_t, const uint8_t*, uint16_t);
DEFINE_FAKE_VALUE_FUNC(glw_imu_error_t, glw_imu_bus_read, uint8_t, uint8_t*, uint16_t);
