#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "driver/driver.h"
#include "config.h"

// implements
#include "glw_led.h"

#define LED_BRIGHTNESS_MAX 255

static unsigned int num_pixels = CONFIG_GLW_LED_NUM_PIXELS;
static int pixel_mapping[] = CONFIG_GLW_LED_PIXEL_MAPPING;

#ifdef BUILD_TESTING
/** Change the default config. Meant for testing purposes. */
void glw_led_change_config(unsigned int new_num_pixels, const int *new_pixel_mapping) {
    // num_pixels can only be changed to a value smaller or equal to the default
    assert(new_num_pixels <= CONFIG_GLW_LED_NUM_PIXELS);

    num_pixels = new_num_pixels;
    memcpy(pixel_mapping, new_pixel_mapping, new_num_pixels * sizeof(int));
}

static const int default_pixel_mapping[] = CONFIG_GLW_LED_PIXEL_MAPPING;
void glw_led_change_config_default(void) {
    num_pixels = CONFIG_GLW_LED_NUM_PIXELS;
    memcpy(pixel_mapping, default_pixel_mapping, sizeof(default_pixel_mapping));
}
#endif

// ---- Current state ----
static bool enabled = true;
static uint8_t brightness = LED_BRIGHTNESS_MAX;
static struct rgb {glw_led_rgb_comp_t r, g, b;} color[CONFIG_GLW_LED_NUM_PIXELS] = {0}; // LED color before intensity scaling
// -----------------------

static void update(void) {
    if(!enabled) return;

    unsigned int red, green, blue;
    for(int i = 0; i < num_pixels; i++){
        red    = (uint16_t)color[i].r * brightness / LED_BRIGHTNESS_MAX;
        green  = (uint16_t)color[i].g * brightness / LED_BRIGHTNESS_MAX;
        blue   = (uint16_t)color[i].b * brightness / LED_BRIGHTNESS_MAX;

        driver_set_pixel_color(red, green, blue, pixel_mapping[i]);
    }

    driver_refresh();
}

// ===== public interface =====

void glw_led_init(void){
    driver_init();
}

void glw_led_set_enabled(bool new_enabled){
    enabled = new_enabled;
    if(enabled) update();
    else { // turn off all LEDs
        for (int i = 0; i < num_pixels; i++)
            driver_set_pixel_color(0, 0, 0, pixel_mapping[i]);

        driver_refresh();
    }
}

void glw_led_set_brightness(uint8_t new_brightness){
    brightness = new_brightness;
    update();
}

void glw_led_set_color(uint8_t red, uint8_t green, uint8_t blue) {
    struct rgb new_color = {red, green, blue};
    for (int i = 0;  i < num_pixels; i++)
        color[i] = new_color;
    update();
}

void glw_led_set_pixel_color(glw_led_rgb_comp_t red, glw_led_rgb_comp_t green, glw_led_rgb_comp_t blue, int index) {
    color[index] = (struct rgb){red, green, blue};
}

void glw_led_update(void) {
    update();
}
