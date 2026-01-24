#include "nvs_flash.h"

#include "glowy_comms.h"
#include "glowy_led.h"


void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    comms_init_t comms_init_struct = {
        .callbacks = {
            .set_enable_cb = glowy_led_set_enabled,
            .set_brightness_cb = glowy_led_set_brightness,
            .set_color_rgb_cb = glowy_led_set_color,
        }
    };
    glowy_comms_init(comms_init_struct);

    glowy_led_init();
}
