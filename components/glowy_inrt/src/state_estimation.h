#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "imu/imu_data.h"

typedef double base_t;
typedef unsigned int inrt_time_t;

typedef struct {
    inrt_time_t release_time; ///< Time of release
    base_t release_velocity;  ///< Estimated vertical velocity at the time of release
} glowy_inrt_throw_t;

/**
 *  Estimated state of the device
 */
typedef struct {
    enum glowy_inrt_state_state_t {
        GLOWY_INRT_STATE_MOVING,   ///< device is moving but not in the air (it may be held or rolling
                                   ///< on a surface)
        GLOWY_INRT_STATE_REST,     ///< device is resting on a static surface
        GLOWY_INRT_STATE_FREEFALL, ///< device is in the air
    } state;

    base_t z_velocity;        ///< Vertical velocity of the device
    base_t look_up_vector[3]; ///< Unit vector pointing up in the IMU's frame.
                              ///< Used to represent the IMU orientation while ignoring rotation in
                              ///< the (world) vertical axis.

    glowy_inrt_throw_t current_throw; ///< When in freefall state, information about the throw

    // -- private
    base_t velocity_average;
    inrt_time_t last_update_time;
} glowy_inrt_state_t;

/**
 * State estimation configuration
 */
typedef struct {
    struct glowy_inrt_freefall_detection_config_t {
        base_t acc_threshold;
        inrt_time_t min_duration_us; ///> How long must the acceleration lie below the threshold
                                     ///> before the device is considered in freefall
    } freefall_detection;

    base_t orientation_correction_rate;

    base_t velocity_avg_decay_rate;
    base_t velocity_avg_correction_rate;
} glowy_inrt_state_estimation_config_t;

static const glowy_inrt_state_estimation_config_t glowy_inrt_config_default = {
    .freefall_detection =
        {
            .acc_threshold = 1.,
            .min_duration_us = 200 * 1000,
        },

    .orientation_correction_rate = 0.005,

    .velocity_avg_decay_rate = 1 - (1. / 3000.),
    .velocity_avg_correction_rate = 0.05,
};

/**
 * Perform an update step
 */
void glowy_inrt_state_update(glowy_inrt_state_t *state, const glowy_inrt_imu_data_t *data,
                             const glowy_inrt_state_estimation_config_t *config);

/**
 * Initialise the state with the first data frame
 */
void glowy_inrt_state_init(glowy_inrt_state_t *state, const glowy_inrt_imu_data_t *data);
