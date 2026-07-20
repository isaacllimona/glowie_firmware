#include "led_strip.h"
#include "config.h"
#include "log.h"

#include "driver.h"

#define LED_GPIO CONFIG_GLW_LED_GPIO
#define LED_NUM  CONFIG_GLW_LED_NUM

static led_strip_handle_t led_handle;

void driver_init(void){
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

int driver_set_pixel_color(driver_rgb_comp_t r, driver_rgb_comp_t g, driver_rgb_comp_t b, int idx) {
    int ret = led_strip_set_pixel(led_handle, idx, r, g, b);
    if (ret != ESP_OK) {
        LOGE("(DRIVER) Error setting pixel number %d, code: %d", idx, ret);
        return 1;
    }
    return 0;
}

int driver_refresh(void) {
    int ret = led_strip_refresh(led_handle);
    if (ret != ESP_OK) {
        LOGE("(DRIVER) Error refreshing LEDs, code: %d", ret);
        return 1;
    }
    return 0;
}
