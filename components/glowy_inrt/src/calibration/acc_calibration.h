#pragma once

#include <stdint.h>

typedef int16_t acc_raw_comp_t;
typedef double calib_t;

calib_t calibration_scales[3][3] = {
    {4.87025070e-04, -1.32536750e-06, -3.62429677e-06},
    {-1.44993072e-06, 4.89917029e-04, -6.26660444e-07},
    {-3.63716007e-06, -1.02635642e-07, 4.88862737e-04},
};

calib_t calibration_offsets[] = {-0.00450271, 0.0828621, 0.01542739};

inline static void acc_calibration_apply(const acc_raw_comp_t raw_data[3],
                                         calib_t calibrated_data[3]) {
    for (int i = 0; i < 3; i++) {
        calibrated_data[i] =
            (raw_data[0] * calibration_scales[i][0] + raw_data[1] * calibration_scales[i][1] +
             raw_data[2] * calibration_scales[i][2] + calibration_offsets[i]) *
            9.81;
    }
}
