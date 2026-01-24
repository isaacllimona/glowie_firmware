#pragma once

#include <stdbool.h>
#include <stdint.h>

void glowy_led_init(void);

/* Enable/disable the LEDs */
void glowy_led_set_enabled(bool enabled);

/* Set color as RGB, with each component ranging from 0-255 */
void glowy_led_set_color(uint8_t red, uint8_t green, uint8_t blue);

/* Set color in HSV space, with h: 0-179, s: 0-255, v: 0-255 */
void glowy_led_set_color_hsv(uint8_t h, uint8_t s, uint8_t v);

/* Set a brightness modifier, from 0-255 */
void glowy_led_set_brightness(uint8_t brightness);
