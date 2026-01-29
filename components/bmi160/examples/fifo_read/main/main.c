#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "bmi160.h"

#define LOG_TAG "main"
#define DATA_BUFFER_SIZE (BMI160_FIFO_BUFFER_SIZE / 7)  // Enough to hold all data from the sensor fifo buffer
#define READ_FREQUENCY_HZ 2

#define DELAY_MS(t) vTaskDelay(t/portTICK_PERIOD_MS)

static void fifo_read_task(void *param) {
    bmi160_init();

    // CONFIG ACC
    bmi160_acc_config_t acc_config = {
        .odr = BMI160_ACC_ODR_25HZ,
        .filter_mode = BMI160_ACC_FILTER_NORMAL
    };
    bmi160_acc_range_t acc_range = BMI160_ACC_RANGE_16G;
    bmi160_acc_enable(acc_config, acc_range);

    // CONFIG GYRO
    bmi160_gyro_config_t gyro_config = {
        .odr = BMI160_GYRO_ODR_25HZ,
        .filter_mode = BMI160_GYRO_FILTER_NORMAL
    };
    bmi160_gyro_range_t gyro_range = BMI160_GYRO_RANGE_2000DPS;
    bmi160_gyro_enable(gyro_config, gyro_range);

    imu_data_t imu_data_buffer[DATA_BUFFER_SIZE];
    bmi160_fifo_init_t fifo_init = {
        .data_buffer = imu_data_buffer,
        .data_buffer_max_size = DATA_BUFFER_SIZE,

        .acc_filtered = true,
        .gyro_filtered = true,
        .acc_downsampling = 2,
        .gyro_downsampling = 2
    };

    bmi160_fifo_t fifo;
    bmi160_fifo_init(&fifo, &fifo_init);
   
    const TickType_t target_period = pdMS_TO_TICKS(1000 / READ_FREQUENCY_HZ);
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(true){
        bmi160_fifo_read(&fifo);

        ESP_LOGI(LOG_TAG, "Read %u data frames from the fifo", fifo.data_buffer_size);
        for(imu_data_t *frame = imu_data_buffer; frame < imu_data_buffer + fifo.data_buffer_size; frame++) {
            ESP_LOGI(LOG_TAG, "acc (x,y,z): %6d, %6d, %6d  |  gyro (x,y,z): %6d, %6d, %6d",
                frame->acc[0], frame->acc[1], frame->acc[2],
                frame->gyro[0], frame->gyro[1], frame->gyro[2]
            );
        }

        vTaskDelayUntil(&lastWakeTime, target_period);
    }
}

void app_main(void) {
    xTaskCreate(
        fifo_read_task,
        "FIFO_READ", 
        5000,  // Must have enough stack for the buffers
        NULL,
        1,
        NULL
    );
}
