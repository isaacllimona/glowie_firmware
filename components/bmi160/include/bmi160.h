#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "bmi160_ll_datatypes.h"

#define BMI160_FIFO_BUFFER_SIZE 1024

/**
* Structure to contain the formatted sensor data
*/
typedef struct {
    enum : uint8_t {
        CONTAINS_GYRO = 0b01,
        CONTAINS_ACC = 0b10
    } flags;
    int16_t gyro[3];
    int16_t acc[3];
} imu_data_t;


/**
 * @brief Initialise the connection to the bmi160 sensor and perform basic checks
**/
int bmi160_init(void);

void bmi160_soft_reset(void);

void bmi160_acc_enable(bmi160_acc_config_t config, bmi160_acc_range_t range);
void bmi160_gyro_enable(bmi160_gyro_config_t config, bmi160_gyro_range_t range);

// ---- FIFO ----

typedef struct {
    imu_data_t *data_buffer;     //> Buffer for the output data
    size_t data_buffer_max_size; //> Number of data frames that fit in the buffer

    bool acc_filtered;
    bool gyro_filtered;
    uint8_t acc_downsampling;  //> Valid range: 0-7. Actual downsampling ratio is 2^acc_downsampling.
    uint8_t gyro_downsampling; //> Valid range: 0-7. Actual downsampling ratio is 2^gyro_downsampling.

    uint8_t fifo_watermark;
} bmi160_fifo_init_t;

typedef struct {
    imu_data_t *data_buffer;
    size_t data_buffer_size;

    size_t data_buffer_max_size;

    uint8_t fifo_buffer[BMI160_FIFO_BUFFER_SIZE];
    uint8_t *fifo_buffer_cursor;
    uint8_t *fifo_buffer_end_pos;
    size_t fifo_buffer_max_size;
} bmi160_fifo_t;

void bmi160_fifo_init(bmi160_fifo_t *fifo, bmi160_fifo_init_t *fifo_init);

/**
 *  Read sensor data from the FIFO.
 *
 *  If the previous read didn't process all data in the internal buffer,
 *  the remaining data will be outputted before fetching new data from the sensor.
 *  Use bmi160_fifo_flush to clear the internal buffer and get new sensor data in the next read.
**/
void bmi160_fifo_read(bmi160_fifo_t *fifo);

/**
 * Clear the internal buffer to get new sensor data in the next read.
 * All data left in the buffer will be lost.
**/
void bmi160_fifo_flush(bmi160_fifo_t *fifo);
