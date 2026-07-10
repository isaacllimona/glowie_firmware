#pragma once

#include <stdint.h>

typedef struct {
    void (*set_color_rgb)(uint8_t r, uint8_t g, uint8_t b);  //< r, g, b between 0-255
    void (*set_color_hsv)(uint8_t h, uint8_t s, uint8_t v);  //< h between 0-159, s, v between 0-255
    unsigned int (*get_time)(void);
} glw_script_init_t;

/** Configure the glw_script module */
void glw_script_init(const glw_script_init_t *init);

/** Start the interpreter and wait for input */
void glw_script_start(void);

/** Stop the interpreter */
void glw_script_stop(void);

/** Queue a script for running */
int glw_script_run_string(const char * script_str);
