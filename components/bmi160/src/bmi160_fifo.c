#include "bmi160_ll.h"
#include "bmi160_ll_datatypes.h"
#include "log.h"

#include "bmi160.h"

void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init) {
    assert(fifo_init->acc_downsampling < 8);  // Must fit in 3 bits
    assert(fifo_init->gyro_downsampling < 8); // Must fit in 3 bits

    fifo->data_buffer_size = 0;

    bmi160_ll_fifo_downsampling_write(fifo_init->acc_filtered, fifo_init->gyro_filtered,
                                      fifo_init->acc_downsampling, fifo_init->gyro_downsampling);

    bmi160_write(BMI160_REG_FIFO_CONFIG_0, fifo_init->fifo_watermark);
    bmi160_write(BMI160_REG_FIFO_CONFIG_1,
                 0xD2); // Enable acc, gyro. Set header mode. Enable timestamp

    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_FIFO_FLUSH);

    // CHECK FOR ERRORS
    uint8_t err = bmi160_read(BMI160_REG_ERR_REG);
    if (err != 0) {
        LOGE("Error: %x", err);
    }
}

static int32_t read_sensortime();
static void read_data_frame(bmi160_fifo_header_t *header, bmi160_imu_data_t *out);

void bmi160_fifo_read(bmi160_fifo_t *fifo) {
    bmi160_imu_data_t *out_cursor = fifo->data_buffer;
    fifo->data_buffer_size = 0;

    bool stop = false;
    do {
        uint8_t header_raw = bmi160_read_keep_active(BMI160_REG_FIFO_DATA);
        bmi160_fifo_header_t *header = (bmi160_fifo_header_t *)&header_raw;

        switch (header->fh_mode) {
        case BMI160_FIFO_FRAME_TYPE_REGULAR:
            if (fifo->data_buffer_size >= BMI160_DATA_BUFFER_SIZE) {
                LOGE("Data buffer is full. FIFO read will be cancelled");
                fifo->data_buffer_size = 0; // Invalidate data, as timestamp would be wrong
                return;
            }

            read_data_frame(header, out_cursor);
            out_cursor++;
            fifo->data_buffer_size++;
            break;

        case BMI160_FIFO_FRAME_TYPE_CONTROL:
            switch (header->fh_parm) {

            case 0b000: // skip frame
                bmi160_read_keep_active(BMI160_REG_FIFO_DATA);
                break;

            case 0b010: // input config frame
                bmi160_read_keep_active(BMI160_REG_FIFO_DATA);
                break;

            case 0b001: // sensortime frame
                fifo->timestamp = read_sensortime();
                stop = true;
                break;
            }

            break;
        }
    } while (!stop);
}

static int32_t read_sensortime() {
    uint8_t data[3];
    bmi160_read_multiple(BMI160_REG_FIFO_DATA, 3, data);
    return (int32_t)data[0] << 16 | (int32_t)data[1] << 8 | (int32_t)data[2];
}

static void read_data_frame(bmi160_fifo_header_t *header, bmi160_imu_data_t *out) {
    if (header->fh_parm & 0b100) {
        LOGE("FIFO contains magnetometer data. This is not supported");
        return;
    }

    uint8_t raw_data_buffer[12];
    unsigned int data_len = (header->fh_parm & 1) + ((header->fh_parm << 1) & 1);
    bmi160_read_multiple_keep_active(BMI160_REG_FIFO_DATA, data_len, raw_data_buffer);

    uint8_t *data_cursor = raw_data_buffer;

    out->flags = 0;

    if (header->fh_parm & 0b010) { // contains gyro data
        out->flags |= BMI160_IMU_DATA_CONTAINS_GYRO;

        bmi160_gyro_data_t *raw_gyro_data = (bmi160_gyro_data_t *)(data_cursor);
        data_cursor += 6;

        out->gyro[0] = (raw_gyro_data->x_msb << 8) | raw_gyro_data->x_lsb;
        out->gyro[1] = (raw_gyro_data->y_msb << 8) | raw_gyro_data->y_lsb;
        out->gyro[2] = (raw_gyro_data->z_msb << 8) | raw_gyro_data->z_lsb;
    }
    if (header->fh_parm & 0b001) { // contains acc data
        out->flags |= BMI160_IMU_DATA_CONTAINS_ACC;

        bmi160_acc_data_t *raw_acc_data = (bmi160_acc_data_t *)(data_cursor);

        out->acc[0] = (raw_acc_data->x_msb << 8) | raw_acc_data->x_lsb;
        out->acc[1] = (raw_acc_data->y_msb << 8) | raw_acc_data->y_lsb;
        out->acc[2] = (raw_acc_data->z_msb << 8) | raw_acc_data->z_lsb;
    }
}
