#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "glw_imu/errors.h"
#include "glw_imu/imu_data.h"
#include "lsm6dsv32x_reg.h"
#include "platform.h"

// implements
#include "glw_imu/fifo.h"

// Take the device context from glw_imu.c
extern stmdev_ctx_t dev_ctx;

// ---- Mapping from sensor ODR to FIFO BDR ----
// The LSM6DSV32X uses the same encoding for ODR and for FIFO batch
// rates, so we simply cast between the enum types.

static lsm6dsv32x_fifo_xl_batch_t odr_to_xl_batch(lsm6dsv32x_data_rate_t odr) {
    return (lsm6dsv32x_fifo_xl_batch_t)odr;
}

static lsm6dsv32x_fifo_gy_batch_t odr_to_gy_batch(lsm6dsv32x_data_rate_t odr) {
    return (lsm6dsv32x_fifo_gy_batch_t)odr;
}

// Fill a data frame from a raw FIFO entry
static void process_fifo_entry(glw_imu_data_frame_t *frame, uint8_t sensor, const uint8_t data[6]) {
    switch (sensor) {
    case 0x01:
        frame->gyro[0] = (int16_t)(data[1] << 8 | data[0]);
        frame->gyro[1] = (int16_t)(data[3] << 8 | data[2]);
        frame->gyro[2] = (int16_t)(data[5] << 8 | data[4]);
        frame->flags |= GLW_IMU_DATA_CONTAINS_GYRO;
        break;

    case 0x02:
        frame->acc[0] = (int16_t)(data[1] << 8 | data[0]);
        frame->acc[1] = (int16_t)(data[3] << 8 | data[2]);
        frame->acc[2] = (int16_t)(data[5] << 8 | data[4]);
        frame->flags |= GLW_IMU_DATA_CONTAINS_ACC;
        break;

    default:
        LOGW("Unexpected FIFO tag 0x%02x – ignoring", sensor);
        break;
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

glw_imu_error_t glw_imu_fifo_init(void) {
    // Read back the sensor ODRs so the FIFO batch rate matches.
    lsm6dsv32x_data_rate_t xl_odr;
    PROPAGATE_ERROR(lsm6dsv32x_xl_data_rate_get(&dev_ctx, &xl_odr));

    lsm6dsv32x_data_rate_t gy_odr;
    PROPAGATE_ERROR(lsm6dsv32x_gy_data_rate_get(&dev_ctx, &gy_odr));

    // Configure batch rates.
    PROPAGATE_ERROR(lsm6dsv32x_fifo_xl_batch_set(&dev_ctx, odr_to_xl_batch(xl_odr)));
    PROPAGATE_ERROR(lsm6dsv32x_fifo_gy_batch_set(&dev_ctx, odr_to_gy_batch(gy_odr)));

    // Enable continuous mode.
    PROPAGATE_ERROR(lsm6dsv32x_fifo_mode_set(&dev_ctx, LSM6DSV32X_STREAM_MODE));

    LOGD("FIFO initialised (accel BDR: 0x%02x, gyro BDR 0x%02x)", xl_odr, gy_odr);

    return GLW_IMU_OK;
}

#define FIFO_RAW_BUFFER_SIZE GLW_IMU_FIFO_BUFFER_SIZE
#define FIFO_RAW_ENTRY_SIZE  7

static struct {
    uint8_t *data_it;
    uint8_t *data_end;
    uint8_t data[FIFO_RAW_BUFFER_SIZE * FIFO_RAW_ENTRY_SIZE];
} fifo_raw_buffer;

glw_imu_error_t glw_imu_fifo_fetch(glw_imu_fifo_fetch_result_t *fetch_result) {
    // ---- Get the fifo fill level
    lsm6dsv32x_fifo_status_t status;
    PROPAGATE_ERROR(lsm6dsv32x_fifo_status_get(&dev_ctx, &status));
    uint16_t fifo_level = status.fifo_level;

    // ---- Read the fifo
    uint16_t read_entries = fifo_level > FIFO_RAW_BUFFER_SIZE ? FIFO_RAW_BUFFER_SIZE : fifo_level;
    uint16_t read_len = read_entries * FIFO_RAW_ENTRY_SIZE;
    PROPAGATE_ERROR(
        lsm6dsv32x_read_reg(&dev_ctx, LSM6DSV32X_FIFO_DATA_OUT_TAG, fifo_raw_buffer.data, read_len));

    // ---- Reset fifo buffer
    fifo_raw_buffer.data_it = fifo_raw_buffer.data;
    fifo_raw_buffer.data_end = fifo_raw_buffer.data + read_len;

    if (fetch_result != NULL) {
        fetch_result->fetched_entries = read_entries;
        fetch_result->available_entries = fifo_level;
        fetch_result->buffer_size = FIFO_RAW_BUFFER_SIZE;
    }

    return GLW_IMU_OK;
}

void glw_imu_fifo_read_frame(glw_imu_data_frame_t *frame) {
    frame->flags = 0;

    if (fifo_raw_buffer.data_it == fifo_raw_buffer.data_end)
        return;

    lsm6dsv32x_fifo_data_out_tag_t *tag = (lsm6dsv32x_fifo_data_out_tag_t *)(&fifo_raw_buffer.data_it[0]);
    uint8_t cnt = tag->tag_cnt;

    do {
        process_fifo_entry(frame, tag->tag_sensor, &fifo_raw_buffer.data_it[1]);

        fifo_raw_buffer.data_it += 7;
        if (fifo_raw_buffer.data_it == fifo_raw_buffer.data_end)
            break;

        tag = (lsm6dsv32x_fifo_data_out_tag_t *)(&fifo_raw_buffer.data_it[0]);
    } while (tag->tag_cnt == cnt);
}
