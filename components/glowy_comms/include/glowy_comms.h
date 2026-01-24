#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t brightness_t;
typedef uint8_t rgb_component_t;

typedef struct comms_callbacks_t{
    void (*set_enable_cb)(bool);
    void (*set_brightness_cb)(brightness_t);
    void (*set_color_rgb_cb)(rgb_component_t, rgb_component_t, rgb_component_t);
} comms_callbacks_t;

typedef struct comms_init_t{
    comms_callbacks_t callbacks;
} comms_init_t;

void glowy_comms_init(comms_init_t init);
