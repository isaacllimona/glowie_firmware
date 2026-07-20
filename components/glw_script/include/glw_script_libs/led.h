#pragma once

#include <stdint.h>

typedef uint8_t glw_script_libs_led_rgb_comp_t;

typedef struct {
    void (*set_color_rgb) (glw_script_libs_led_rgb_comp_t r, glw_script_libs_led_rgb_comp_t g, glw_script_libs_led_rgb_comp_t b);
    void (*set_pixel_color_rgb)(glw_script_libs_led_rgb_comp_t r, glw_script_libs_led_rgb_comp_t g, glw_script_libs_led_rgb_comp_t b, int idx);
    void (*update)(void);
} glw_script_libs_led_init_t;

void glw_script_libs_led_add(const glw_script_libs_led_init_t *init);
