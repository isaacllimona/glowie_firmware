#include <stddef.h>

#include "glw_imu/errors.h"
#include "glw_imu/imu_config.h"
#include "glw_imu/imu_data.h"
#include "glw_os/time.h"

#include "data_dispatcher_priv.h"
#include "glw_imu.h"
#include "glw_imu/fifo.h"
#include "imu/imu_data.h"
#include "log.h"
#include "state_estimation.h"

// implements
#include "glw_inrt/glw_inrt.h"

const static glw_imu_odr_t imu_odr = GLW_IMU_ODR_3840Hz;
const static int imu_odr_val_hz = 3840;
const static unsigned int imu_read_period_us = 1000000 / imu_odr_val_hz;

static unsigned int data_time = 0;

const static unsigned int update_frequency_hz = 60;

void glw_inrt_init(void) {
    glw_imu_init();
    const glw_imu_accel_config_t accel_config = {.full_scale = GLW_IMU_ACCEL_FS_32G, .odr = imu_odr};
    const glw_imu_gyro_config_t gyro_config = {.full_scale = GLW_IMU_GYRO_FS_4000DPS, .odr = imu_odr};
    glw_imu_configure_accel(&accel_config);
    glw_imu_configure_gyro(&gyro_config);
    glw_imu_fifo_init();

    glw_inrt_dispatcher_init();
}

static void imu_data_to_inrt(glw_inrt_imu_data_t *inrt_data, const glw_imu_data_frame_t *imu_data,
                             glw_inrt_imu_time_t timestamp) {
    inrt_data->flags = imu_data->flags;
    inrt_data->acc[0] = imu_data->acc[0];
    inrt_data->acc[1] = imu_data->acc[1];
    inrt_data->acc[2] = imu_data->acc[2];
    inrt_data->gyro[0] = imu_data->gyro[0];
    inrt_data->gyro[1] = imu_data->gyro[1];
    inrt_data->gyro[2] = imu_data->gyro[2];
    inrt_data->timestamp = timestamp;
}

void glw_inrt_run(void) {
    glw_imu_data_frame_t imu_data;
    glw_inrt_imu_data_t inrt_imu_data;
    glw_inrt_state_t inrt_state;

    glw_imu_fifo_fetch_result_t fetch_result;
    glw_imu_error_t err;

    // ---- Get first IMU data and initialise state

    glw_imu_fifo_fetch(&fetch_result);
    while (fetch_result.fetched_entries == 0) {
        LOGW("No IMU data was read. Retrying...");
        os_delay(1000 / update_frequency_hz);
        glw_imu_fifo_fetch(&fetch_result);
    }

    // skip to last data frame
    glw_imu_data_frame_t imu_data_aux;
    glw_imu_data_frame_t *imu_data_ptr1 = &imu_data, *imu_data_ptr2 = &imu_data_aux, *imu_data_ptr_aux;
    do {
        glw_imu_fifo_read_frame(imu_data_ptr1);
        imu_data_ptr_aux = imu_data_ptr1;
        imu_data_ptr1 = imu_data_ptr2;
        imu_data_ptr2 = imu_data_ptr_aux;
    } while (imu_data_ptr2->flags != 0);

    imu_data_to_inrt(&inrt_imu_data, imu_data_ptr1, data_time);
    data_time += imu_read_period_us;
    glw_inrt_state_init(&inrt_state, &inrt_imu_data);

    // ---- State estimation loop

    os_timer timer;
    os_timer_init(&timer, update_frequency_hz);

    LOGD("entering state estimation loop");

    while (1) {
        os_timer_sleep(&timer);

        err = glw_imu_fifo_fetch(&fetch_result);
        if (err != GLW_IMU_OK) {
            LOGE("Error fetching data from fifo. Error code: %i", err);
            continue;
        }

        if (fetch_result.fetched_entries == (fetch_result.buffer_size - 20)) {
            LOGW("IMU buffer is full. Data may be lost.");
        }

        while (1) {
            glw_imu_fifo_read_frame(&imu_data);
            if (imu_data.flags == 0)
                break;

            imu_data_to_inrt(&inrt_imu_data, &imu_data, data_time);
            data_time += imu_read_period_us;

            glw_inrt_state_update(&inrt_state, &inrt_imu_data, &glw_inrt_config_default);
            glw_inrt_dispatcher_update(&inrt_state);
        }
    }
}
