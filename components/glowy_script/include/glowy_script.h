#pragma once

#include <stdint.h>
#include <stdbool.h>

/** Configuration for the LED library */
typedef struct {
    void (*set_color_rgb)(uint8_t r, uint8_t g, uint8_t b);  //< r, g, b between 0-255
    void (*set_color_hsv)(uint8_t h, uint8_t s, uint8_t v);  //< h between 0-159, s, v between 0-255
} lua_led_lib_cfg_t;


/** Start the interpreter and wait for input */
void glowy_script_start(void);

/** Queue a script for running */
bool glowy_script_run_string(char * script_str);


// ==== LIBRARIES ====

/** Configure the LED library. Should be done before starting the interpreter */
void lua_led_lib_cfg(const lua_led_lib_cfg_t *cfg);
