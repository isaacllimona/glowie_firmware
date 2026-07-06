#pragma once

#include <stdint.h>

typedef int16_t gyro_raw_comp_t;
typedef double calib_t;

const calib_t gyro_sensitivity = 140e-3; // dps/LSB
const calib_t gyro_sensitivity_rad = gyro_sensitivity * 3.1416 / 180.;
const calib_t gyro_offset[3] = {0, 0, 0};

inline static void gyro_calibration_apply(const gyro_raw_comp_t raw_data[3],
                                          calib_t calibrated_data[3]) {
    for (int i = 0; i < 3; i++) {
        calibrated_data[i] = raw_data[i] * gyro_sensitivity_rad + gyro_offset[i];
    }
}
