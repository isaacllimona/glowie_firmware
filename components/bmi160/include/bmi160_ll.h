#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

// clang-format off

/**
 * @brief BMI160 register addresses
**/
typedef enum: uint8_t {
    /* Device Identification */
    BMI160_REG_CHIP_ID         = 0x00, // Default value: 0xD1
    
    /* Error and Status Registers */
    BMI160_REG_ERR_REG         = 0x02,
    BMI160_REG_PMU_STATUS      = 0x03,
    
    /* Data Registers */
    BMI160_REG_MAG_DATA        = 0x04, // 0x04 to 0x0B (8 bytes)
    BMI160_REG_GYRO_DATA       = 0x0C, // 0x0C to 0x11 (6 bytes)
    BMI160_REG_ACCEL_DATA      = 0x12, // 0x12 to 0x17 (6 bytes)
    
    /* Sensor Time */
    BMI160_REG_SENSORTIME      = 0x18, // 0x18 to 0x1A (3 bytes)

    /* Status */
    BMI160_REG_STATUS          = 0x1B,
    BMI160_REG_INT_STATUS_0    = 0x1C,
    BMI160_REG_INT_STATUS_1    = 0x1D,
    BMI160_REG_INT_STATUS_2    = 0x1E,
    BMI160_REG_INT_STATUS_3    = 0x1F,
    
    /* Temperature */
    BMI160_REG_TEMPERATURE     = 0x20, // 0x20 to 0x21 (2 bytes)

    /* FIFO data */
    BMI160_REG_FIFO_LENGTH     = 0x22, // 0x22 to 0x23 (2 bytes)
    BMI160_REG_FIFO_DATA       = 0x24,
    
    /* Sensor configuration */
    BMI160_REG_ACCEL_CONF      = 0x40,
    BMI160_REG_ACCEL_RANGE     = 0x41,
    BMI160_REG_GYRO_CONF       = 0x42,
    BMI160_REG_GYRO_RANGE      = 0x43,
    BMI160_REG_MAG_CONF        = 0x44,

    /* FIFO configuration */
    BMI160_REG_FIFO_DOWNS      = 0x45,
    BMI160_REG_FIFO_CONFIG_0   = 0x46,
    BMI160_REG_FIFO_CONFIG_1   = 0x47,
    
    /* Auxiliary Interface Configuration */
    BMI160_REG_MAG_IF_0        = 0x4B,
    BMI160_REG_MAG_IF_1        = 0x4C,
    BMI160_REG_MAG_IF_2        = 0x4D,
    BMI160_REG_MAG_IF_3        = 0x4E,
    BMI160_REG_MAG_IF_4        = 0x4F,
    
    /* Interrupt Configuration */
    BMI160_REG_INT_EN_0        = 0x50,
    BMI160_REG_INT_EN_1        = 0x51,
    BMI160_REG_INT_EN_2        = 0x52,
    BMI160_REG_INT_OUT_CTRL    = 0x53,
    BMI160_REG_INT_LATCH       = 0x54,
    BMI160_REG_INT_MAP_0       = 0x55,
    BMI160_REG_INT_MAP_1       = 0x56,
    BMI160_REG_INT_MAP_2       = 0x57,
    BMI160_REG_INT_DATA_0      = 0x58,
    BMI160_REG_INT_DATA_1      = 0x59,
    BMI160_REG_INT_LOWHIGH_0   = 0x5A,
    BMI160_REG_INT_LOWHIGH_1   = 0x5B,
    BMI160_REG_INT_LOWHIGH_2   = 0x5C,
    BMI160_REG_INT_LOWHIGH_3   = 0x5D,
    BMI160_REG_INT_LOWHIGH_4   = 0x5E,
    BMI160_REG_INT_MOTION_0    = 0x5F,
    BMI160_REG_INT_MOTION_1    = 0x60,
    BMI160_REG_INT_MOTION_2    = 0x61,
    BMI160_REG_INT_MOTION_3    = 0x62,
    BMI160_REG_INT_TAP_0       = 0x63,
    BMI160_REG_INT_TAP_1       = 0x64,
    BMI160_REG_INT_ORIENT_0    = 0x65,
    BMI160_REG_INT_ORIENT_1    = 0x66,
    BMI160_REG_INT_FLAT_0      = 0x67,
    BMI160_REG_INT_FLAT_1      = 0x68,
    
    /* Misc configuration */
    BMI160_REG_FOC_CONF        = 0x69,
    BMI160_REG_CONF            = 0x6A,
    BMI160_REG_IF_CONF         = 0x6B,
    BMI160_REG_PMU_TRIGGER     = 0x6C,
    BMI160_REG_SELF_TEST       = 0x6D,
    BMI160_REG_NV_CONF         = 0x70,
    
    /* Sensor offsets */
    BMI160_REG_OFFSET          = 0x71, // 0x71 to 0x77 (7 bytes)

    /* Step counter */
    BMI160_REG_STEP_CNT        = 0x78, // 0x78 to 0x79 (2 bytes)
    BMI160_REG_STEP_CONF_0     = 0x7A,
    BMI160_REG_STEP_CONF_1     = 0x7B,
    
    /* Command Register */
    BMI160_REG_COMMAND         = 0x7E 
} bmi160_reg_t;

// clang-format on

/**
 * @brief Initialise the interface on the ESP side
 **/
void bmi160_interface_init(void);

/**
 * @brief Set up the interface on the BMI160 side. Must be called again after a BMI160 soft reset.
 **/
void bmi160_interface_start(void);

void bmi160_write(bmi160_reg_t addr, uint8_t data);
uint8_t bmi160_read(bmi160_reg_t addr);
void bmi160_read_multiple(bmi160_reg_t addr, unsigned int length, void *buffer);

uint8_t bmi160_read_keep_active(bmi160_reg_t addr);
void bmi160_read_multiple_keep_active(bmi160_reg_t addr, unsigned int length, void *buffer);

// ==== Register access abstractions ====

static inline void bmi160_ll_fifo_downsampling_write(
    bool acc_filtered, bool gyro_filtered,
    uint8_t acc_downsampling, ///> Valid range: 0-7. Actual downsampling ratio is 2^acc_downsampling.
    uint8_t gyro_downsampling ///> Valid range: 0-7. Actual downsampling ratio is 2^gyro_downsampling.
) {
    assert(acc_downsampling < 8);  // Must fit in 3 bits
    assert(gyro_downsampling < 8); // Must fit in 3 bits

    bmi160_write(BMI160_REG_FIFO_DOWNS, (acc_filtered ? 0x80 : 0x00)  | (acc_downsampling << 4) |
                                        (gyro_filtered ? 0x08 : 0x00) | (gyro_downsampling));
}

static inline uint16_t bmi160_ll_fifo_length_read() {
    uint8_t fifo_length_regs[2];
    bmi160_read_multiple(BMI160_REG_FIFO_LENGTH, 2, fifo_length_regs);
    return ((uint16_t)(fifo_length_regs[1] & 0x07) << 8) + fifo_length_regs[0];
}

static inline void bmi160_ll_fifo_data_read(uint8_t *out_buffer, size_t len) {
    bmi160_read_multiple(BMI160_REG_FIFO_DATA, len, out_buffer);
}
