#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "glowy_comms.h"
#include "glowy_led.h"
#include "glowy_script.h"


static void delay_func(unsigned int time_ms) {
    vTaskDelay(time_ms / portTICK_PERIOD_MS);
}

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

    glowy_script_init_t script_init = {
        .set_color_rgb = glowy_led_set_color,
        .set_color_hsv = glowy_led_set_color_hsv,
        .delay = delay_func
    };
    glowy_script_init(&script_init);

    run_script(
        " h = 0 "
        " while true do "
        "   h = h >= 180 and 0 or h + 2 "
        "   led.setColorHSV(h, 255, 100) "
        "   sys.sleep(30) "
        " end "
    );
}
