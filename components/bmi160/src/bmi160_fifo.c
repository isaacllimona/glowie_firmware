#include "bmi160_ll.h"
#include "bmi160_ll_datatypes.h"
#include "log.h"

#include "bmi160.h"

#define FIFO_BUFFER_SIZE 1024

void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init) {
    assert(fifo_init->acc_downsampling < 8);  // Must fit in 3 bits
    assert(fifo_init->gyro_downsampling < 8); // Must fit in 3 bits

    fifo->data_buffer_size = 0;

    bmi160_ll_fifo_downsampling_write(fifo_init->acc_filtered, fifo_init->gyro_filtered,
                                      fifo_init->acc_downsampling, fifo_init->gyro_downsampling);

    bmi160_write(BMI160_REG_FIFO_CONFIG_0, fifo_init->fifo_watermark);
    bmi160_write(BMI160_REG_FIFO_CONFIG_1, 0xD0); // Enable acc, gyro. Set header mode.

    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_FIFO_FLUSH);

    // CHECK FOR ERRORS
    uint8_t err = bmi160_read(BMI160_REG_ERR_REG);
    if (err != 0) {
        LOGE("Error: %x", err);
    }
}

static uint16_t read_fifo(uint8_t *out_buffer);
static void parse_fifo(uint8_t *fifo_buffer, uint16_t fifo_buffer_len, bmi160_fifo_t *fifo);

void bmi160_fifo_read(bmi160_fifo_t *fifo) {
    uint8_t fifo_buffer[FIFO_BUFFER_SIZE];
    uint16_t read_len = read_fifo(fifo_buffer);
    parse_fifo(fifo_buffer, read_len, fifo);
}

static uint16_t read_fifo(uint8_t *out_buffer) {
    uint16_t read_len = bmi160_ll_fifo_length_read();
    if (read_len == 0)
        return 0;
    bmi160_ll_fifo_data_read(out_buffer, read_len);
    return read_len;
}

static void parse_fifo(uint8_t *fifo_buffer, uint16_t fifo_buffer_len, bmi160_fifo_t *fifo) {
    if (fifo_buffer_len == 0) {
        fifo->data_buffer_size = 0;
        fifo->skipped_frames = 0;
        return;
    }

    bmi160_imu_data_t *out_cursor = fifo->data_buffer;
    uint8_t *fifo_cursor = fifo_buffer;
    bmi160_fifo_header_t *header;
    fifo->data_buffer_size = 0;

    // Check for skip frame
    header = (bmi160_fifo_header_t *)(fifo_cursor);
    if (header->fh_mode == BMI160_FIFO_FRAME_TYPE_CONTROL && header->fh_parm == 0b000) {
        fifo->skipped_frames = *(fifo_cursor + 1);
        fifo_cursor += 2;
    } else { // No skipped frames
        fifo->skipped_frames = 0;
    }

    while (fifo_cursor < fifo_buffer + fifo_buffer_len) {
        if (*fifo_cursor == 0x80) { // overread
            LOGE("FIFO was overread");
            return;
        }

        header = (bmi160_fifo_header_t *)(fifo_cursor);
        fifo_cursor += 1;
        switch (header->fh_mode) {
        case BMI160_FIFO_FRAME_TYPE_REGULAR:
            out_cursor->flags = 0;

            if (header->fh_parm & 0b100) { // contains mag data
                LOGW("FIFO contains magnetometer data. It will be ignored");
                fifo_cursor += 8;
            }
            if (header->fh_parm & 0b010) { // contains gyro data
                out_cursor->flags |= BMI160_IMU_DATA_CONTAINS_GYRO;

                bmi160_gyro_data_t *raw_gyro_data = (bmi160_gyro_data_t *)(fifo_cursor);
                fifo_cursor += 6;

                out_cursor->gyro[0] = (raw_gyro_data->x_msb << 8) | raw_gyro_data->x_lsb;
                out_cursor->gyro[1] = (raw_gyro_data->y_msb << 8) | raw_gyro_data->y_lsb;
                out_cursor->gyro[2] = (raw_gyro_data->z_msb << 8) | raw_gyro_data->z_lsb;
            }
            if (header->fh_parm & 0b001) { // contains acc data
                out_cursor->flags |= BMI160_IMU_DATA_CONTAINS_ACC;

                bmi160_acc_data_t *raw_acc_data = (bmi160_acc_data_t *)(fifo_cursor);
                fifo_cursor += 6;

                out_cursor->acc[0] = (raw_acc_data->x_msb << 8) | raw_acc_data->x_lsb;
                out_cursor->acc[1] = (raw_acc_data->y_msb << 8) | raw_acc_data->y_lsb;
                out_cursor->acc[2] = (raw_acc_data->z_msb << 8) | raw_acc_data->z_lsb;
            }

            out_cursor++;
            fifo->data_buffer_size++;
            break;

        case BMI160_FIFO_FRAME_TYPE_CONTROL:
            switch (header->fh_parm) {
            case 0b000: // skip frame
                LOGE("Unexpected skip frame in the middle of the fifo");
                fifo_cursor++;
                break;
            case 0b010: // input config frame
                LOGW("Input config frame found in FIFO. Will be ignored.");
                fifo_cursor++;
                break;
            case 0b001: // sensortime frame
                LOGE("Unexpected sensortime frame");
                return;
            }
            break;
        }
    }
}
