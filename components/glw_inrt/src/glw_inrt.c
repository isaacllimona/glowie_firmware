#include <stddef.h>

#include "glw_common/os/time.h"

#include "data_dispatcher_priv.h"
#include "imu/imu.h"
#include "state_estimation.h"
#include "log.h"

// implements
#include "glw_inrt/glw_inrt.h"

const static unsigned int update_frequency_hz = 60;

void glw_inrt_init(void) {
    glw_inrt_imu_init();
    glw_inrt_dispatcher_init();
}

void glw_inrt_run(void) {
    size_t imu_data_len;
    glw_inrt_imu_data_t imu_data;
    glw_inrt_state_t inrt_state;

    // ---- Get first IMU data and initialise state

    imu_data_len = glw_inrt_imu_fetch();
    while (imu_data_len == 0) {
        LOGW("No IMU was read. Retrying...");
        os_delay(1000 / update_frequency_hz);
        imu_data_len = glw_inrt_imu_fetch();
    }

    for (int i = 0; i < imu_data_len; i++) // skip to last data frame
        glw_inrt_imu_next(&imu_data);

    glw_inrt_state_init(&inrt_state, &imu_data);

    // ---- State estimation loop

    os_timer timer;
    os_timer_init(&timer, update_frequency_hz);

    while (1) {
        os_timer_sleep(&timer);

        imu_data_len = glw_inrt_imu_fetch();

        if (imu_data_len == 0) {
            LOGW("No IMU data available");
        }

        for (int i = 0; i < imu_data_len; i++) {
            glw_inrt_imu_next(&imu_data);
            glw_inrt_state_update(&inrt_state, &imu_data, &glw_inrt_config_default);
            glw_inrt_dispatcher_update(&inrt_state);
        }
    }
}
