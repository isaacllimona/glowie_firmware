#pragma once

#include <stdint.h>

typedef int16_t acc_raw_comp_t;
typedef double calib_t;

const calib_t scale = 0.976e-3; // g/LSB

calib_t calibration_scales[3][3] = {
    {scale, 0, 0},
    {0, scale, 0},
    {0, 0, scale},
};

calib_t calibration_offsets[] = {0., 0., 0.};

inline static void acc_calibration_apply(const acc_raw_comp_t raw_data[3], calib_t calibrated_data[3]) {
    for (int i = 0; i < 3; i++) {
        calibrated_data[i] =
            (raw_data[0] * calibration_scales[i][0] + raw_data[1] * calibration_scales[i][1] +
             raw_data[2] * calibration_scales[i][2] + calibration_offsets[i]) *
            9.81;
    }
}
