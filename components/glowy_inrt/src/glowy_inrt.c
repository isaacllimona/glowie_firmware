#include <stddef.h>

#include "glowy_common/os/time.h"

#include "data_dispatcher_priv.h"
#include "imu/imu.h"
#include "state_estimation.h"
#include "log.h"

// implements
#include "glowy_inrt/glowy_inrt.h"

const static unsigned int update_frequency_hz = 60;

void glowy_inrt_init(void) {
    glowy_inrt_imu_init();
    glowy_inrt_dispatcher_init();
}

void glowy_inrt_run(void) {
    size_t imu_data_len;
    glowy_inrt_imu_data_t imu_data;
    glowy_inrt_state_t inrt_state;

    // ---- Get first IMU data and initialise state

    imu_data_len = glowy_inrt_imu_fetch();
    while (imu_data_len == 0) {
        LOGW("No IMU was read. Retrying...");
        os_delay(1000 / update_frequency_hz);
        imu_data_len = glowy_inrt_imu_fetch();
    }

    for (int i = 0; i < imu_data_len; i++) // skip to last data frame
        glowy_inrt_imu_next(&imu_data);

    glowy_inrt_state_init(&inrt_state, &imu_data);

    // ---- State estimation loop

    os_timer timer;
    os_timer_init(&timer, update_frequency_hz);

    while (1) {
        os_timer_sleep(&timer);

        imu_data_len = glowy_inrt_imu_fetch();

        if (imu_data_len == 0) {
            LOGW("No IMU data available");
        }

        for (int i = 0; i < imu_data_len; i++) {
            glowy_inrt_imu_next(&imu_data);
            glowy_inrt_state_update(&inrt_state, &imu_data, &glowy_inrt_config_default);
            glowy_inrt_dispatcher_update(&inrt_state);
        }
    }
}
