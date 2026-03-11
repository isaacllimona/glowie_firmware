#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bmi160_ll_datatypes.h"

// Sensor fifo has a max capacity of 1024 bytes
// Data frames take at least 7 bytes
#define BMI160_DATA_BUFFER_SIZE ((1024 / 7))

/**
 * Structure to contain the formatted sensor data
 */
typedef struct {
    enum : uint8_t {
        BMI160_IMU_DATA_CONTAINS_GYRO = 0b01,
        BMI160_IMU_DATA_CONTAINS_ACC = 0b10,
    } flags;
    int16_t gyro[3];
    int16_t acc[3];
} bmi160_imu_data_t;

/**
 * @brief Initialise the connection to the bmi160 sensor and perform basic checks
 **/
int bmi160_init(void);

void bmi160_soft_reset(void);

void bmi160_acc_enable(bmi160_acc_config_t config, bmi160_acc_range_t range);
void bmi160_gyro_enable(bmi160_gyro_config_t config, bmi160_gyro_range_t range);

// ---- FIFO ----

typedef struct {
    bool acc_filtered;
    bool gyro_filtered;
    uint8_t acc_downsampling;  ///> Valid range: 0-7. Actual downsampling ratio is 2^acc_downsampling.
    uint8_t gyro_downsampling; ///> Valid range: 0-7. Actual downsampling ratio is 2^gyro_downsampling.

    uint8_t fifo_watermark;
} bmi160_fifo_init_t;

typedef struct {
    bmi160_imu_data_t data_buffer[BMI160_DATA_BUFFER_SIZE];
    size_t data_buffer_size; ///> Number of data frames available in the buffer
    uint8_t skipped_frames;  ///> In case of a fifo overflow, number of frames that have been dropped since the
                             ///> last readout
} bmi160_fifo_t;

void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init);

/**
 *  Read sensor data from the FIFO.
 **/
void bmi160_fifo_read(bmi160_fifo_t *fifo);
