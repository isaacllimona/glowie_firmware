#include <stdbool.h>

#include "glw_log.h" // IWYU pragma: export

#include "led_strip.h"
#include "config.h"

#include "glw_led.h"

SET_LOG_TAG("glw_led");

#define LED_GPIO CONFIG_GLW_LED_GPIO
#define LED_NUM  CONFIG_GLW_LED_NUM

#define LED_BRIGHTNESS_MAX 255

static bool enabled = true;
static uint8_t brightness = LED_BRIGHTNESS_MAX;
static struct {uint8_t r, g, b;} color = {0}; // LED color before intensity scaling

static led_strip_handle_t led_handle;

void glw_led_init(void){
    const led_strip_config_t strip_config = {
        .led_model = LED_MODEL_WS2812,
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_NUM
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(
        led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle)
    );

    led_strip_clear(led_handle);
}

static inline void update(void) {
    if(!enabled) return;

    uint32_t red    = (uint16_t)color.r * brightness / LED_BRIGHTNESS_MAX;
    uint32_t green  = (uint16_t)color.g * brightness / LED_BRIGHTNESS_MAX;
    uint32_t blue   = (uint16_t)color.b * brightness / LED_BRIGHTNESS_MAX;

    esp_err_t ret;
    for(int i = 0; i < LED_NUM; i++){
        ret = led_strip_set_pixel(led_handle, i, red, green, blue);
        if(ret != ESP_OK) LOGE("Error setting pixel number %d, code: %d", i, ret);
    }

    ret = led_strip_refresh(led_handle);
    if(ret != ESP_OK) LOGE("Error refreshing LEDs, code: %d", ret);
}

void glw_led_set_enabled(bool new_enabled){
    enabled = new_enabled;
    if(enabled) update();
    else led_strip_clear(led_handle);
}

void glw_led_set_color(uint8_t red, uint8_t green, uint8_t blue){
    color.r = red;
    color.g = green;
    color.b = blue;
    update();
}

void glw_led_set_color_hsv(uint8_t h, uint8_t s, uint8_t v){
    int r, g, b;

    int i = h / 30;
	int f = h - i * 30;
	int p = (uint32_t)v * (255 - s) / 255;
	int q = (uint32_t)v * (255 - f * s / 30) / 255;
	int t = (uint32_t)v * (255 - (30 - f) * s / 30) / 255;
	
	switch (i % 6) {
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
        default: return;
	}

    color.r = r;
	color.g = g;
	color.b = b;
    update();
}

void glw_led_set_brightness(uint8_t new_brightness){
    brightness = new_brightness;
    update();
}
