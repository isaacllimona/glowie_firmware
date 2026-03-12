#include "esp_log.h"

#include "bmi160/bmi160.h"
#include "bmi160/ll/interface.h"

#define LOG_TAG "main"

void app_main(void) {
    bmi160_init();

    int chip_id = bmi160_read(BMI160_REG_CHIP_ID);
    ESP_LOGI(LOG_TAG, "Got chip id: 0x%x (expected 0xd1)", chip_id);
}
