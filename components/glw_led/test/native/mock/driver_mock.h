#pragma once

#include <stdint.h>
#include <fff.h>

#include "driver/driver.h"

DECLARE_FAKE_VOID_FUNC(driver_init);
DECLARE_FAKE_VALUE_FUNC(int, driver_set_pixel_color, driver_rgb_comp_t, driver_rgb_comp_t, driver_rgb_comp_t, int);
DECLARE_FAKE_VALUE_FUNC(int, driver_refresh);

inline static void reset_all_fakes(void) {
    RESET_FAKE(driver_init);
    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);
}
