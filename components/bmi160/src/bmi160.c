#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "bmi160.h"

#define DELAY_MS(t) vTaskDelay(t/portTICK_PERIOD_MS)
#define LOG_TAG "BMI160"


int bmi160_init(void){
    bmi160_interface_init();
    bmi160_interface_start();
    DELAY_MS(10);
    
    // Check chip_id to identify connection problems
    uint8_t chip_id = bmi160_read(BMI160_REG_CHIP_ID);
    if(chip_id != 0xd1){
        ESP_LOGE(LOG_TAG, "Unexpected chip id. Chip may not be connected or powered");
        return 1;
    }

    // Check fatal error flag
    uint8_t error_reg = bmi160_read(BMI160_REG_ERR_REG);
    if(error_reg & 1){
        ESP_LOGE(LOG_TAG, "Fatal error: Chip not operable, broken hardware");
        return 1;
    }
    
    return 0;
}
