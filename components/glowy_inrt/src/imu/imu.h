/**
 * IMU access abstraction layer
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "imu_data.h"

/**
 * Initialise and configure the IMU
 */
void glowy_inrt_imu_init(void);

/**
 * Configure the IMU.
 *
 * Use this instead of glowy_inrt_imu_init if the IMU has already been
 * initialised outside the glowy_inrt module
 */
void glowy_inrt_imu_config(void);

/**
 * Fetch all available data from the IMU
 *
 * Returns the number of data frames available
 */
size_t glowy_inrt_imu_fetch(void);

/**
 * Read the next data frame
 */
void glowy_inrt_imu_next(glowy_inrt_imu_data_t *out);
