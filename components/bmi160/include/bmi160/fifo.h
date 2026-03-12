#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "imu_data.h"

// Sensor fifo has a max capacity of 1024 bytes
// Data frames take at least 7 bytes
#define BMI160_FIFO_DATA_BUFFER_SIZE ((1024 / 7))

typedef struct {
    bool acc_filtered;
    bool gyro_filtered;
    uint8_t acc_downsampling;  ///> Valid range: 0-7. Actual downsampling ratio is 2^acc_downsampling.
    uint8_t gyro_downsampling; ///> Valid range: 0-7. Actual downsampling ratio is 2^gyro_downsampling.

    uint8_t fifo_watermark;
} bmi160_fifo_init_t;

typedef struct {
    bmi160_imu_data_t data_buffer[BMI160_FIFO_DATA_BUFFER_SIZE];
    size_t data_buffer_size; ///> Number of data frames available in the buffer
    uint8_t skipped_frames;  ///> In case of a fifo overflow, number of frames that have been dropped since the
                             ///> last readout
} bmi160_fifo_t;

void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init);

/**
 *  Read sensor data from the FIFO.
 **/
void bmi160_fifo_read(bmi160_fifo_t *fifo);
