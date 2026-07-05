#include "bus/bus.h"
#include "lsm6dsv32x_reg.h"
#include "platform.h"

// implements
#include "glw_imu.h"

static int32_t bus_write(void *handle, uint8_t addr, const uint8_t *buffer, uint16_t len) {
    glw_imu_bus_write(addr, buffer, len);
}

static int32_t bus_read(void *handle, uint8_t addr, uint8_t *buffer, uint16_t len) {
    glw_imu_bus_read(addr, buffer, len);
}

glw_imu_error_t glw_imu_init(void) {
    PROPAGATE_ERROR(glw_imu_bus_init());

    stmdev_ctx_t dev_ctx = {
        .write_reg = bus_write,
        .read_reg = bus_read,
    };

    uint8_t device_id;
    PROPAGATE_ERROR(lsm6dsv32x_device_id_get(&dev_ctx, &device_id));
    ASSERT_OR_RETURN(device_id == LSM6DSV32X_ID, GLW_IMU_DEVICE_ERROR);

    return GLW_IMU_OK;
}
