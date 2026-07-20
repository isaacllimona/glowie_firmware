#pragma once

typedef unsigned int driver_rgb_comp_t;

void driver_init(void);

/**
 * Set the color of a pixel
 *
 * r, g and b in the range 0-255
 * driver_refresh() must be called to apply the change
 */
int driver_set_pixel_color(driver_rgb_comp_t r, driver_rgb_comp_t g, driver_rgb_comp_t b, int idx);
int driver_refresh(void);
