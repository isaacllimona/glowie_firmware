#include <fff.h>

#include "driver/driver.h"

#include "driver_mock.h"

DEFINE_FAKE_VOID_FUNC(driver_init);
DEFINE_FAKE_VALUE_FUNC(int, driver_set_pixel_color, driver_rgb_comp_t, driver_rgb_comp_t, driver_rgb_comp_t, int);
DEFINE_FAKE_VALUE_FUNC(int, driver_refresh);
