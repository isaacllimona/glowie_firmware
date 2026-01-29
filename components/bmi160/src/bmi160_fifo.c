#include "bmi160_ll_datatypes.h"
#include "esp_log.h"

#include "bmi160.h"
#include "bmi160_ll.h"


#define LOG_TAG "BMI160"


void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init) {
    assert(fifo_init->acc_downsampling < 8);  // Must fit in 3 bits
    assert(fifo_init->gyro_downsampling < 8);  // Must fit in 3 bits

    fifo->data_buffer = fifo_init->data_buffer;
    fifo->data_buffer_max_size = fifo_init->data_buffer_max_size;
    fifo->data_buffer_size = 0;

    fifo->fifo_buffer_max_size = BMI160_FIFO_BUFFER_SIZE;
    fifo->fifo_buffer_cursor = fifo->fifo_buffer;
    fifo->fifo_buffer_end_pos = fifo->fifo_buffer;

    bmi160_ll_fifo_downsampling_write(
        fifo_init->acc_filtered, fifo_init->gyro_filtered,
        fifo_init->acc_downsampling, fifo_init->gyro_downsampling
    );

    bmi160_write(BMI160_REG_FIFO_CONFIG_0, fifo_init->fifo_watermark);
    bmi160_write(BMI160_REG_FIFO_CONFIG_1, 0xD0);  // Enable acc, gyro. Set header mode.

    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_FIFO_FLUSH);

    // CHECK FOR ERRORS
    uint8_t err = bmi160_read(BMI160_REG_ERR_REG);
    if(err != 0){
        ESP_LOGE(LOG_TAG, "Error: %x", err);
    }
}

static uint16_t read_fifo_buffer(uint8_t *out_buffer, uint16_t out_max_len);
static void parse_fifo(uint8_t **fifo_cursor, uint8_t *fifo_end,
    imu_data_t *out_buffer, size_t *out_len, size_t max_out_len);

void bmi160_fifo_read(bmi160_fifo_t *fifo) {
    fifo->data_buffer_size = 0;

    if(fifo->fifo_buffer_cursor < fifo->fifo_buffer_end_pos) {  // Some data left in the fifo buffer
        parse_fifo(&fifo->fifo_buffer_cursor, fifo->fifo_buffer_end_pos,
            fifo->data_buffer, &fifo->data_buffer_size, fifo->data_buffer_max_size);
    }

    if(fifo->fifo_buffer_cursor == fifo->fifo_buffer_end_pos) {
        // All data in the fifo buffer has been parsed. Get new data from the sensor.
        uint16_t fifo_buffer_size = read_fifo_buffer(fifo->fifo_buffer, fifo->fifo_buffer_max_size);
        fifo->fifo_buffer_end_pos = fifo->fifo_buffer + fifo_buffer_size;
        fifo->fifo_buffer_cursor = fifo->fifo_buffer;

        parse_fifo(&fifo->fifo_buffer_cursor, fifo->fifo_buffer_end_pos,
        fifo->data_buffer + fifo->data_buffer_size, &fifo->data_buffer_size, fifo->data_buffer_max_size);
    }
}

static uint16_t read_fifo_buffer(uint8_t *out_buffer, uint16_t out_max_len) {
    uint16_t read_len = bmi160_ll_fifo_length_read();
    read_len = read_len > out_max_len ? out_max_len : read_len;
    if(read_len == 0) return 0;
    bmi160_ll_fifo_data_read(out_buffer, read_len);
    return read_len;
}

static void parse_fifo(uint8_t **fifo_cursor, uint8_t *fifo_end,
    imu_data_t *out_buffer, size_t *out_len, size_t max_out_len)
{
    while(*fifo_cursor < fifo_end && *out_len < max_out_len) {
        if(**fifo_cursor == 0x80){  // overread
            ESP_LOGW(LOG_TAG, "FIFO was overread");
            return;
        }

        bmi160_fifo_header_t *header = (bmi160_fifo_header_t*)(*fifo_cursor);
        *fifo_cursor += 1;
        switch(header->fh_mode){
            case BMI160_FIFO_FRAME_TYPE_REGULAR:
                out_buffer->flags = 0;

                if(header->fh_parm & 0b100){  // contains mag data
                    ESP_LOGW(LOG_TAG, "FIFO contains magnetometer data. It will be ignored");
                    *fifo_cursor += 8;
                }
                if(header->fh_parm & 0b010){  // contains gyro data
                    out_buffer->flags |= CONTAINS_GYRO;

                    bmi160_gyro_data_t *raw_gyro_data = (bmi160_gyro_data_t*)(*fifo_cursor);
                    *fifo_cursor += 6;

                    out_buffer->gyro[0] = (raw_gyro_data->x_msb << 8) | raw_gyro_data->x_lsb;
                    out_buffer->gyro[1] = (raw_gyro_data->y_msb << 8) | raw_gyro_data->y_lsb;
                    out_buffer->gyro[2] = (raw_gyro_data->z_msb << 8) | raw_gyro_data->z_lsb;
                }
                if(header->fh_parm & 0b001){  // contains acc data
                    out_buffer->flags |= CONTAINS_ACC;

                    bmi160_acc_data_t *raw_acc_data = (bmi160_acc_data_t*)(*fifo_cursor);
                    *fifo_cursor += 6;

                    out_buffer->acc[0] = (raw_acc_data->x_msb << 8) | raw_acc_data->x_lsb;
                    out_buffer->acc[1] = (raw_acc_data->y_msb << 8) | raw_acc_data->y_lsb;
                    out_buffer->acc[2] = (raw_acc_data->z_msb << 8) | raw_acc_data->z_lsb;
                }

                out_buffer++;
                (*out_len)++;

                break;
            case BMI160_FIFO_FRAME_TYPE_CONTROL:
                *fifo_cursor += 1; // skip control frame data byte
                break;
        }
    }
}

void bmi160_fifo_flush(bmi160_fifo_t *fifo) {
    fifo->fifo_buffer_cursor = fifo->fifo_buffer;
    fifo->fifo_buffer_end_pos = fifo->fifo_buffer;
}
