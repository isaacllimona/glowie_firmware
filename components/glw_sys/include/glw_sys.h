#pragma once

#include <stdbool.h>

/*
 * Initialise the glowie power system
 * Should be called immediately after boot
 */
void glw_sys_init(void);

/*
 * Power off the glowie
 * If usb is connected, glowie will shut down as soon as it gets disconnected
 */
void glw_sys_shutdown(void);

/*
 * Power on/off the LEDs
 */
void glw_sys_led_en(bool enabled);
