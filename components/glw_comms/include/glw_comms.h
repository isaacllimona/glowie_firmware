#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t brightness_t;
typedef uint8_t rgb_component_t;

/** Callbacks for receiving data from the host */
typedef struct {
    void (*set_enable_cb)(bool);
    void (*set_brightness_cb)(brightness_t);
    void (*set_color_rgb_cb)(rgb_component_t, rgb_component_t, rgb_component_t);

    bool (*script_in_cb)(char *input);  //< get the received input as a null-terminated string.
                                        //< Return false if script cannot be processed.
} glw_comms_callbacks_t;

typedef struct comms_init_t{
    glw_comms_callbacks_t callbacks;
} glw_comms_init_t;

void glw_comms_init(glw_comms_init_t init);
