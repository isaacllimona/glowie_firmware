#pragma once

#include <stdint.h>

typedef int16_t gyro_raw_comp_t;
typedef double calib_t;

calib_t gyro_sensitivity = 16.6;
calib_t gyro_offset[3] = {0.005, 0.005, -0.005};

inline static void gyro_calibration_apply(const gyro_raw_comp_t raw_data[3],
                                          calib_t calibrated_data[3]) {
    for (int i = 0; i < 3; i++) {
        calibrated_data[i] = raw_data[i] / gyro_sensitivity * (3.1416 / 180.) + gyro_offset[i];
    }
}
