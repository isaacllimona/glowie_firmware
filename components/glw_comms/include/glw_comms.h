#pragma once

#include <strings.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void (*led_en)(bool enabled);
    void (*brightness_set)(uint8_t value);
    void (*shutdown)(void);
    void (*script_run)(const char* script);
    void (*time_set)(unsigned int ms);
} glw_comms_callbacks_t;

typedef struct comms_init_t {
    glw_comms_callbacks_t callbacks;
} glw_comms_init_t;

void glw_comms_init(glw_comms_init_t init);

void glw_comms_stop(void);

void glw_comms_send_err(const char *prefix, const char *msg);
