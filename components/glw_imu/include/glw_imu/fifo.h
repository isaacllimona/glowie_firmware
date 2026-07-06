#pragma once

#include "glw_imu/errors.h"
#include "glw_imu/imu_data.h"

typedef struct {
    /** Number of entries available in the FIFO before the fetch */
    unsigned int available_entries;

    /**
     * Number of entried fetched from the FIFO.
     * Will be lower than available_entries if the whole FIFO did not fit in the internal buffer.
     * Each entry corresponds to a single sensor reading (accel/gyro).
     * Therefore this number is not the amount of frames that can be read with glw_imu_fifo_read_frame().
     * To read all frames, glw_imu_fifo_read_frame() should be read until an empty frame is returned
     */
    unsigned int fetched_entries;

    /** Size of the internal buffer used to store FIFO entries */
    unsigned int buffer_size;
} glw_imu_fifo_fetch_result_t;

/**
 * Initialise the FIFO to store accelerometer and gyroscope data
 *
 * The batch rate for each sensor is automatically derived from the sensor's
 * currently configured ODR.
 */
glw_imu_error_t glw_imu_fifo_init(void);

/**
 * Fetch fifo data from the sensor
 *
 * The data will be stored internally and can be retrieved with glw_imu_fifo_read().
 * Previously fetched data will be discarded.
 * The amount of fetched data is limited by the size of the internal buffer.
 */
glw_imu_error_t glw_imu_fifo_fetch(glw_imu_fifo_fetch_result_t *fetch_result);

/**
 * Get the next data frame from the FIFO
 *
 * Before using this function, data must be fetched from the sensor with glw_imu_fifo_fetch()
 * If all fetched data has been read, frame->flags is set to 0
 */
void glw_imu_fifo_read_frame(glw_imu_data_frame_t *frame);
