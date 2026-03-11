#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "portmacro.h"

#include "bmi160.h"

#define LOG_TAG "imu_read"
#define DELAY_MS(t) vTaskDelay(t/portTICK_PERIOD_MS)

static void imu_read_task(void *params){
    MessageBufferHandle_t message_buffer = (MessageBufferHandle_t)(params);

    bmi160_init();

    // CONFIG ACC
    bmi160_acc_config_t acc_config = {
        .odr = BMI160_ACC_ODR_800HZ,
        .filter_mode = BMI160_ACC_FILTER_NORMAL
    };
    bmi160_acc_range_t acc_range = BMI160_ACC_RANGE_16G;
    bmi160_acc_enable(acc_config, acc_range);

    // CONFIG GYRO
    bmi160_gyro_config_t gyro_config = {
        .odr = BMI160_GYRO_ODR_800HZ,
        .filter_mode = BMI160_GYRO_FILTER_NORMAL
    };
    bmi160_gyro_range_t gyro_range = BMI160_GYRO_RANGE_2000DPS;
    bmi160_gyro_enable(gyro_config, gyro_range);

    // INIT FIFO
    bmi160_fifo_init_t fifo_init = {
        .acc_filtered = true,
        .gyro_filtered = true,
        .acc_downsampling = 0,
        .gyro_downsampling = 0
    };

    bmi160_fifo_t fifo;
    bmi160_fifo_init(&fifo, &fifo_init);
   
    const size_t data_frame_size_bytes = sizeof(bmi160_imu_data_t);
    const TickType_t target_period = pdMS_TO_TICKS(10);
    TickType_t lastWakeTime = xTaskGetTickCount();

    for(;;){
        bmi160_fifo_read(&fifo);

        if (fifo.skipped_frames != 0) {
            ESP_LOGW("main__imu_read", "%d IMU data frames were dropped", fifo.skipped_frames);
        }

        for (bmi160_imu_data_t *it = fifo.data_buffer; it < fifo.data_buffer + fifo.data_buffer_size; it++) {
            xMessageBufferSend(message_buffer, (void*)it, data_frame_size_bytes, 0);
        }

        vTaskDelayUntil(&lastWakeTime, target_period);
    }

    vTaskDelete(NULL);
}

void imu_read_start(MessageBufferHandle_t message_buffer) {
    xTaskCreate(
        imu_read_task,
        "IMU_READ", 
        10000,
        (void*)message_buffer,
        1,
        NULL
    );
}


