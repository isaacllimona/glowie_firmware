#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t glw_led_rgb_comp_t;

void glw_led_init(void);

/* Enable/disable the LEDs */
void glw_led_set_enabled(bool enabled);

/* Set a brightness modifier, from 0-255 */
void glw_led_set_brightness(uint8_t brightness);

/**
 * Set the color of all pixels
 *
 * The color is given as RGB, with each component ranging from 0-255.
 */
void glw_led_set_color(glw_led_rgb_comp_t red, glw_led_rgb_comp_t green, glw_led_rgb_comp_t blue);

/**
 * Set the color of a single pixel.
 *
 * The color is given as RGB, with each component ranging from 0-255.
 * The color change is not applied automatically.
 * Call glw_led_update() after setting all pixels to apply the change.
 */
void glw_led_set_pixel_color(glw_led_rgb_comp_t red, glw_led_rgb_comp_t green, glw_led_rgb_comp_t blue, int index);
void glw_led_update(void);
