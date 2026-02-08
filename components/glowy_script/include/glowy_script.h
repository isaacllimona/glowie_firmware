#pragma once

#include <stdint.h>

typedef unsigned int glowy_script_time_t;

typedef struct {
    void (*set_color_rgb)(uint8_t r, uint8_t g, uint8_t b);  //< r, g, b between 0-255
    void (*set_color_hsv)(uint8_t h, uint8_t s, uint8_t v);  //< h between 0-159, s, v between 0-255

    void (*delay)(glowy_script_time_t time_ms);
} glowy_script_init_t;

void glowy_script_init(const glowy_script_init_t *init);

void run_script(char * script);
