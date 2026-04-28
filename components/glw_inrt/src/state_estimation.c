#include <math.h>
#include <stdbool.h>

#include "calibration/acc_calibration.h"
#include "calibration/gyro_calibration.h"
#include "imu/imu_data.h"
#include "log.h"

// implements
#include "state_estimation.h"

static base_t vec_norm(const base_t v[3]) { return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

static base_t vec_normalize(const base_t v[3], base_t result[3]) {
    base_t norm = vec_norm(v);
    result[0] = v[0] / norm;
    result[1] = v[1] / norm;
    result[2] = v[2] / norm;

    return norm;
}

static void cross_product(const base_t v[3], const base_t u[3], base_t result[3]) {
    result[0] = v[1] * u[2] - v[2] * u[1];
    result[1] = v[2] * u[0] - v[0] * u[2];
    result[2] = v[0] * u[1] - v[1] * u[0];
}

static base_t dot_product(const base_t v[3], const base_t u[3]) {
    return v[0] * u[0] + v[1] * u[1] + v[2] * u[2];
}

/// Applies rodrigues' rotation formula to rotate vector v by axis and angle
static void rodrigues_rotation(base_t v[3], const base_t axis[3], const base_t angle) {
    base_t c = cos(angle);
    base_t s = sin(angle);

    base_t axis_cross_v[3];
    cross_product(axis, v, axis_cross_v);
    base_t axis_dot_v_times_1_minus_c = dot_product(axis, v) * (1. - c);

    v[0] = v[0] * c + axis_cross_v[0] * s + axis[0] * axis_dot_v_times_1_minus_c;
    v[1] = v[1] * c + axis_cross_v[1] * s + axis[1] * axis_dot_v_times_1_minus_c;
    v[2] = v[2] * c + axis_cross_v[2] * s + axis[2] * axis_dot_v_times_1_minus_c;
}

static void freefall_detection(glw_inrt_state_t *state, base_t acc_norm, inrt_time_t time,
                               const struct glw_inrt_freefall_detection_config_t *config) {
    if (state->state == GLW_INRT_STATE_FREEFALL) {
        if (acc_norm > config->acc_threshold) {
            state->state = GLW_INRT_STATE_MOVING;
            state->current_throw.release_time = 0;
        }
    } else {
        if (acc_norm <= config->acc_threshold) {
            if (state->current_throw.release_time == 0) {
                state->current_throw.release_time = time;
                state->current_throw.release_velocity = state->z_velocity;
            } else {
                if (time >= state->current_throw.release_time + config->min_duration_us) {
                    state->state = GLW_INRT_STATE_FREEFALL;
                }
            }
        } else {
            if (state->current_throw.release_time != 0)
                state->current_throw.release_time = 0;
        }
    }
}

void glw_inrt_state_update(glw_inrt_state_t *state, const glw_inrt_imu_data_t *data,
                             const glw_inrt_state_estimation_config_t *config) {
    // ---- Calibration
    if (abs(data->acc[0]) > glw_inrt_imu_consts.acc_max ||
        abs(data->acc[1]) > glw_inrt_imu_consts.acc_max ||
        abs(data->acc[2]) > glw_inrt_imu_consts.acc_max) {
        LOGW("Accelerometer clipped");
    }

    if (abs(data->gyro[0]) > glw_inrt_imu_consts.gyro_max ||
        abs(data->gyro[1]) > glw_inrt_imu_consts.gyro_max ||
        abs(data->gyro[2]) > glw_inrt_imu_consts.gyro_max) {
        LOGW("Gyro clipped");
    }

    base_t acc_calib[3];
    base_t gyr_calib[3];
    acc_calibration_apply(data->acc, acc_calib);
    gyro_calibration_apply(data->gyro, gyr_calib);

    // ---- Compute common things to be used across the function

    base_t acc_norm = vec_norm(acc_calib);

    base_t gyr_axis[3];
    base_t gyr_norm = vec_normalize(gyr_calib, gyr_axis);

    // ---- Freefall detection

    freefall_detection(state, acc_norm, data->timestamp, &config->freefall_detection);

    // ---- Compute time step

    base_t dt = (data->timestamp - state->last_update_time) / 1000000.; // in seconds

    // ---- Update orientation

    if (gyr_norm > 0)
        rodrigues_rotation(state->look_up_vector, gyr_axis, -gyr_norm * dt);

    // correct orientation with accelerometer

    const base_t r = config->orientation_correction_rate;
    if (acc_norm < 10. && acc_norm > 9.6) {
        base_t acc_axis[3] = {acc_calib[0] / acc_norm, acc_calib[1] / acc_norm, acc_calib[2] / acc_norm};

        state->look_up_vector[0] = (1. - r) * state->look_up_vector[0] + r * acc_axis[0];
        state->look_up_vector[1] = (1. - r) * state->look_up_vector[1] + r * acc_axis[1];
        state->look_up_vector[2] = (1. - r) * state->look_up_vector[2] + r * acc_axis[2];
    }
    vec_normalize(state->look_up_vector, state->look_up_vector);

    // ---- Integrate velocity

    base_t acc_z = dot_product(acc_calib, state->look_up_vector);
    state->z_velocity = state->z_velocity + (acc_z - 9.81) * dt;

    // ---- Drive velocity average to zero

    state->velocity_average = state->velocity_average * config->velocity_avg_decay_rate +
                              state->z_velocity * (1. - config->velocity_avg_decay_rate);

    if (state->state != GLW_INRT_STATE_FREEFALL) {
        base_t vel_delta = state->velocity_average * config->velocity_avg_correction_rate;
        state->z_velocity -= vel_delta;
        state->velocity_average -= vel_delta;
    }

    // ---- Finish up

    state->last_update_time = data->timestamp;
}

void glw_inrt_state_init(glw_inrt_state_t *state, const glw_inrt_imu_data_t *data) {
    // ---- Calibration

    base_t acc_calib[3];
    base_t gyr_calib[3];
    acc_calibration_apply(data->acc, acc_calib);
    gyro_calibration_apply(data->gyro, gyr_calib);

    // ---- Set initial orientation with accelerometer

    vec_normalize(acc_calib, state->look_up_vector);

    // ---- Set other initial values

    state->state = GLW_INRT_STATE_MOVING;
    state->z_velocity = 0;
    state->velocity_average = 0;
    state->last_update_time = data->timestamp;
}
