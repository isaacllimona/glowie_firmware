#include "bmi160/ll/interface.h"
#include "os.h"
#include "log.h"

// implements
#include "bmi160/bmi160.h"

int bmi160_init(void) {
    bmi160_interface_init();
    bmi160_interface_start();
    DELAY_MS(10);

    // Check chip_id to identify connection problems
    uint8_t chip_id = bmi160_read(BMI160_REG_CHIP_ID);
    if (chip_id != 0xd1) {
        LOGE("Unexpected chip id. Chip may not be connected or powered");
        return 1;
    }

    // Check fatal error flag
    uint8_t error_reg = bmi160_read(BMI160_REG_ERR_REG);
    if (error_reg & 1) {
        LOGE("Fatal error: Chip not operable, broken hardware");
        return 1;
    }

    return 0;
}

void bmi160_acc_enable(bmi160_acc_config_t config, bmi160_acc_range_t range) {
    if (config.us) {
        LOGW("Undersampling is not supported");
    }

    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_ACCEL_SET_PMU_NORMAL);
    DELAY_MS(10);

    bmi160_write(BMI160_REG_ACCEL_CONF, config.byte);
    bmi160_write(BMI160_REG_ACCEL_RANGE, range);
    bmi160_read(BMI160_REG_ACCEL_DATA); // to clear data
}

void bmi160_gyro_enable(bmi160_gyro_config_t config, bmi160_gyro_range_t range) {
    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_GYRO_SET_PMU_NORMAL);
    DELAY_MS(100);

    bmi160_write(BMI160_REG_GYRO_CONF, config.byte);
    bmi160_write(BMI160_REG_GYRO_RANGE, range);
    bmi160_read(BMI160_REG_GYRO_DATA); // to clear data
}

void bmi160_soft_reset(void) {
    bmi160_write(BMI160_REG_COMMAND, BMI160_CMD_SOFT_RESET);
    bmi160_interface_start();
}
