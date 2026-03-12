#include "freertos/idf_additions.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "portmacro.h"

#include "bmi160/imu_data.h"

#define SENSOR_FIFO_BUFFER_SIZE (1024)
#define IMU_DATA_BUFFER_SIZE (SENSOR_FIFO_BUFFER_SIZE / 7)  // Each data frame in the buffer will be at least 7 bytes long
#define IMU_DATA_BUFFER_SIZE_BYTES (IMU_DATA_BUFFER_SIZE * sizeof(bmi160_imu_data_t))
#define TCP_DATA_BUFFER_SIZE_BYTES 1024

#define LOG_TAG "main"

void tcp_server_start(MessageBufferHandle_t message_buffer);
void imu_read_start(MessageBufferHandle_t message_buffer);

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    MessageBufferHandle_t imu_message_buffer = xMessageBufferCreate(IMU_DATA_BUFFER_SIZE_BYTES);
    MessageBufferHandle_t tcp_message_buffer = xMessageBufferCreate(TCP_DATA_BUFFER_SIZE_BYTES);

    imu_read_start(imu_message_buffer);
    tcp_server_start(tcp_message_buffer);

    bmi160_imu_data_t data_frame;
    char tx_data[43];

    while(1) {
        size_t data_len_bytes = xMessageBufferReceive(imu_message_buffer, (void *)&data_frame, IMU_DATA_BUFFER_SIZE_BYTES, portMAX_DELAY);
        assert(data_len_bytes == sizeof(bmi160_imu_data_t));

        size_t frame_size;
        if(data_frame.flags & BMI160_IMU_DATA_CONTAINS_ACC){
            frame_size = sprintf(tx_data, "%d,%d,%d;%d,%d,%d\n",
                data_frame.gyro[0], data_frame.gyro[1], data_frame.gyro[2],
                data_frame.acc[0], data_frame.acc[1], data_frame.acc[2]);
        }
        else{
            frame_size = sprintf(tx_data, "%d,%d,%d;\n", data_frame.gyro[0], data_frame.gyro[1], data_frame.gyro[2]);
        }

        xMessageBufferSend(tcp_message_buffer, tx_data, frame_size, portMAX_DELAY);
    }
}
