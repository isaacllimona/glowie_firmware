#include "bus/bus.h"
#include "lsm6dsv32x_reg.h"
#include "platform.h"

// implements
#include "glw_imu.h"

static int32_t bus_write(void *handle, uint8_t addr, const uint8_t *buffer, uint16_t len) {
    return glw_imu_bus_write(addr, buffer, len);
}

static int32_t bus_read(void *handle, uint8_t addr, uint8_t *buffer, uint16_t len) {
    return glw_imu_bus_read(addr, buffer, len);
}

stmdev_ctx_t dev_ctx = {
    .write_reg = bus_write,
    .read_reg = bus_read,
};

// --------------------------------------------------------------------------------
// Mapping functions to map from generic glw_imu configuration to LSM6DSV32X values
// --------------------------------------------------------------------------------

static lsm6dsv32x_data_rate_t map_odr(glw_imu_odr_t odr) {
    switch (odr) {
    case GLW_IMU_ODR_OFF:
        return LSM6DSV32X_ODR_OFF;
    case GLW_IMU_ODR_1Hz875:
        return LSM6DSV32X_ODR_AT_1Hz875;
    case GLW_IMU_ODR_7Hz5:
        return LSM6DSV32X_ODR_AT_7Hz5;
    case GLW_IMU_ODR_15Hz:
        return LSM6DSV32X_ODR_AT_15Hz;
    case GLW_IMU_ODR_30Hz:
        return LSM6DSV32X_ODR_AT_30Hz;
    case GLW_IMU_ODR_60Hz:
        return LSM6DSV32X_ODR_AT_60Hz;
    case GLW_IMU_ODR_120Hz:
        return LSM6DSV32X_ODR_AT_120Hz;
    case GLW_IMU_ODR_240Hz:
        return LSM6DSV32X_ODR_AT_240Hz;
    case GLW_IMU_ODR_480Hz:
        return LSM6DSV32X_ODR_AT_480Hz;
    case GLW_IMU_ODR_960Hz:
        return LSM6DSV32X_ODR_AT_960Hz;
    case GLW_IMU_ODR_1920Hz:
        return LSM6DSV32X_ODR_AT_1920Hz;
    case GLW_IMU_ODR_3840Hz:
        return LSM6DSV32X_ODR_AT_3840Hz;
    case GLW_IMU_ODR_7680Hz:
        return LSM6DSV32X_ODR_AT_7680Hz;
    default:
        return LSM6DSV32X_ODR_OFF;
    }
}

static lsm6dsv32x_xl_full_scale_t map_accel_fs(glw_imu_accel_fs_t fs) {
    switch (fs) {
    case GLW_IMU_ACCEL_FS_4G:
        return LSM6DSV32X_4g;
    case GLW_IMU_ACCEL_FS_8G:
        return LSM6DSV32X_8g;
    case GLW_IMU_ACCEL_FS_16G:
        return LSM6DSV32X_16g;
    case GLW_IMU_ACCEL_FS_32G:
        return LSM6DSV32X_32g;
    default:
        return LSM6DSV32X_4g;
    }
}

static lsm6dsv32x_gy_full_scale_t map_gyro_fs(glw_imu_gyro_fs_t fs) {
    switch (fs) {
    case GLW_IMU_GYRO_FS_125DPS:
        return LSM6DSV32X_125dps;
    case GLW_IMU_GYRO_FS_250DPS:
        return LSM6DSV32X_250dps;
    case GLW_IMU_GYRO_FS_500DPS:
        return LSM6DSV32X_500dps;
    case GLW_IMU_GYRO_FS_1000DPS:
        return LSM6DSV32X_1000dps;
    case GLW_IMU_GYRO_FS_2000DPS:
        return LSM6DSV32X_2000dps;
    case GLW_IMU_GYRO_FS_4000DPS:
        return LSM6DSV32X_4000dps;
    default:
        return LSM6DSV32X_250dps;
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

glw_imu_error_t glw_imu_init(void) {
    PROPAGATE_ERROR(glw_imu_bus_init());

    uint8_t device_id;
    PROPAGATE_ERROR(lsm6dsv32x_device_id_get(&dev_ctx, &device_id));
    ASSERT_OR_RETURN(device_id == LSM6DSV32X_ID, GLW_IMU_DEVICE_ERROR);

    return GLW_IMU_OK;
}

glw_imu_error_t glw_imu_configure_gyro(const glw_imu_gyro_config_t *config) {
    PROPAGATE_ERROR(lsm6dsv32x_gy_full_scale_set(&dev_ctx, map_gyro_fs(config->full_scale)));
    PROPAGATE_ERROR(lsm6dsv32x_gy_data_rate_set(&dev_ctx, map_odr(config->odr)));

    return GLW_IMU_OK;
}

glw_imu_error_t glw_imu_configure_accel(const glw_imu_accel_config_t *config) {
    PROPAGATE_ERROR(lsm6dsv32x_xl_full_scale_set(&dev_ctx, map_accel_fs(config->full_scale)));
    PROPAGATE_ERROR(lsm6dsv32x_xl_data_rate_set(&dev_ctx, map_odr(config->odr)));

    return GLW_IMU_OK;
}
