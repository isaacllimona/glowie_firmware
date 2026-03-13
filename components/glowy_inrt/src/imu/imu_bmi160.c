#include "bmi160/bmi160.h"
#include "bmi160/fifo.h"

#include "log.h"

// implements
#include "imu_data.h"
#include "imu.h"

#define GYRO_FREQ (1600)
#define GYRO_ODR  BMI160_GYRO_ODR_1600HZ

#define ACC_FREQ (1600)
#define ACC_ODR  BMI160_ACC_ODR_1600HZ

#define COPY_3VEC(src, dst)                                                                                  \
    dst[0] = src[0];                                                                                         \
    dst[1] = src[1];                                                                                         \
    dst[2] = src[2]

const struct glowy_inrt_imu_consts glowy_inrt_imu_consts = {
    .acc_max = 32766,
    .gyro_max = 32766,
};

static const unsigned int fifo_freq = GYRO_FREQ > ACC_FREQ ? GYRO_FREQ : ACC_FREQ;
static const unsigned int fifo_period_us = 1000000 / fifo_freq;

static struct {
    bmi160_fifo_t fifo;
    bmi160_imu_data_t *data_cursor;
    bmi160_imu_data_t *data_end;
    unsigned int cursor_time;
    unsigned int end_time; ///< timestamp of the frame that would come right after the last data frame
} reader;

void glowy_inrt_imu_init() {
    bmi160_init();
    glowy_inrt_imu_config();
}

void glowy_inrt_imu_config() {
    // CONFIG ACC
    bmi160_acc_config_t acc_config = {.odr = ACC_ODR, .filter_mode = BMI160_ACC_FILTER_NORMAL};
    bmi160_acc_range_t acc_range = BMI160_ACC_RANGE_16G;
    bmi160_acc_enable(acc_config, acc_range);

    // CONFIG GYRO
    bmi160_gyro_config_t gyro_config = {.odr = GYRO_ODR, .filter_mode = BMI160_GYRO_FILTER_NORMAL};
    bmi160_gyro_range_t gyro_range = BMI160_GYRO_RANGE_2000DPS;
    bmi160_gyro_enable(gyro_config, gyro_range);

    // SET UP FIFO
    bmi160_fifo_init_t fifo_init = {
        .acc_filtered = true, .gyro_filtered = true, .acc_downsampling = 0, .gyro_downsampling = 0};

    bmi160_fifo_init(&reader.fifo, &fifo_init);
}

size_t glowy_inrt_imu_fetch() {
    bmi160_fifo_read(&reader.fifo);

    if (reader.fifo.skipped_frames == 255) {
        LOGW("Too many data frames were dropped. Timestamps may get shifted");
    }

    reader.cursor_time = reader.end_time + fifo_period_us * reader.fifo.skipped_frames;
    reader.end_time = reader.cursor_time + fifo_period_us * reader.fifo.data_buffer_size;

    reader.data_cursor = reader.fifo.data_buffer;
    reader.data_end = reader.fifo.data_buffer + reader.fifo.data_buffer_size;

    return reader.fifo.data_buffer_size;
}

void glowy_inrt_imu_next(glowy_inrt_imu_data_t *out) {
    if (reader.data_cursor == reader.data_end) {
        out->flags = 0;
        return;
    }

    out->flags = reader.data_cursor->flags;
    COPY_3VEC(reader.data_cursor->acc, out->acc);
    COPY_3VEC(reader.data_cursor->gyro, out->gyro);
    out->timestamp = reader.cursor_time;

    reader.data_cursor++;
    reader.cursor_time += fifo_period_us;
}
