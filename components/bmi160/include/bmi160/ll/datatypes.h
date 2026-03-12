#pragma once

#include <stdint.h>

// ---- DATA ----

typedef struct {
    uint8_t x_lsb;
    uint8_t x_msb;

    uint8_t y_lsb;
    uint8_t y_msb;

    uint8_t z_lsb;
    uint8_t z_msb;
} bmi160_gyro_data_t, bmi160_acc_data_t, bmi160_mag_data_t;

// ---- FIFO DATA ----

typedef struct {
    unsigned int fh_ext : 2;
    unsigned int fh_parm : 4;
    enum {
        BMI160_FIFO_FRAME_TYPE_REGULAR = 0b10,
        BMI160_FIFO_FRAME_TYPE_CONTROL = 0b01
    } fh_mode : 2;
} bmi160_fifo_header_t;

// ---- SENSOR CONFIGURATION ----

typedef union {
    struct {
        enum {
            BMI160_ACC_ODR_0_78HZ   = 0x1,
            BMI160_ACC_ODR_1_56HZ   = 0x2,
            BMI160_ACC_ODR_3_12HZ   = 0x3,
            BMI160_ACC_ODR_6_25HZ   = 0x4,
            BMI160_ACC_ODR_12_5HZ   = 0x5,
            BMI160_ACC_ODR_25HZ     = 0x6,
            BMI160_ACC_ODR_50HZ     = 0x7,
            BMI160_ACC_ODR_100HZ    = 0x8,
            BMI160_ACC_ODR_200HZ    = 0x9,
            BMI160_ACC_ODR_400HZ    = 0xA,
            BMI160_ACC_ODR_800HZ    = 0xB,
            BMI160_ACC_ODR_1600HZ   = 0xC
        } odr : 4;
        union {
            enum {
                BMI160_ACC_FILTER_NORMAL     = 0x2,
                BMI160_ACC_FILTER_OSR2       = 0x1,
                BMI160_ACC_FILTER_OSR4       = 0x0,
            } filter_mode : 3;
            enum {
                BMI160_ACC_AVERAGING_1    = 0x0,
                BMI160_ACC_AVERAGING_2    = 0x1,
                BMI160_ACC_AVERAGING_4    = 0x2,
                BMI160_ACC_AVERAGING_8    = 0x3,
                BMI160_ACC_AVERAGING_16   = 0x4,
                BMI160_ACC_AVERAGING_32   = 0x5,
                BMI160_ACC_AVERAGING_64   = 0x6,
                BMI160_ACC_AVERAGING_128  = 0x7
            } averaging_cycles : 3;
        };
        enum {
            BMI160_ACC_UNDERSAMPLING_DISABLED   = 0x0,
            BMI160_ACC_UNDERSAMPLING_ENABLED    = 0x1
        } us : 1;
    };
    uint8_t byte;
} bmi160_acc_config_t;

typedef enum : uint8_t {
    BMI160_ACC_RANGE_2G     = 0x03,
    BMI160_ACC_RANGE_4G     = 0x05,
    BMI160_ACC_RANGE_8G     = 0x08,
    BMI160_ACC_RANGE_16G    = 0x0C
} bmi160_acc_range_t;

typedef union {
    struct {
        enum {
            BMI160_GYRO_ODR_25HZ     = 0x6,
            BMI160_GYRO_ODR_50HZ     = 0x7,
            BMI160_GYRO_ODR_100HZ    = 0x8,
            BMI160_GYRO_ODR_200HZ    = 0x9,
            BMI160_GYRO_ODR_400HZ    = 0xA,
            BMI160_GYRO_ODR_800HZ    = 0xB,
            BMI160_GYRO_ODR_1600HZ   = 0xC,
            BMI160_GYRO_ODR_3200HZ   = 0xD
        } odr : 4;
        enum {
            BMI160_GYRO_FILTER_NORMAL     = 0x2,
            BMI160_GYRO_FILTER_OSR2       = 0x1,
            BMI160_GYRO_FILTER_OSR4       = 0x0,
        } filter_mode : 4;
    };
    uint8_t byte;
} bmi160_gyro_config_t;

typedef enum : uint8_t {
    BMI160_GYRO_RANGE_2000DPS   = 0x00,
    BMI160_GYRO_RANGE_1000DPS   = 0x01,
    BMI160_GYRO_RANGE_500DPS    = 0x02,
    BMI160_GYRO_RANGE_250DPS    = 0x03,
    BMI160_GYRO_RANGE_125DPS    = 0x04
} bmi160_gyro_range_t;

// ---- COMMANDS ----

/**
 * @brief Commands that can be written to BMI160_REG_COMMAND register
**/
typedef enum: uint8_t {
    /* Power Mode Commands */
    BMI160_CMD_ACCEL_SET_PMU_SUSPEND      = 0x10,
    BMI160_CMD_ACCEL_SET_PMU_NORMAL       = 0x11,
    BMI160_CMD_ACCEL_SET_PMU_LOWPOWER     = 0x12,
    
    BMI160_CMD_GYRO_SET_PMU_SUSPEND       = 0x14,
    BMI160_CMD_GYRO_SET_PMU_NORMAL        = 0x15,
    BMI160_CMD_GYRO_SET_PMU_FAST_STARTUP  = 0x17,
    
    BMI160_CMD_MAG_SET_PMU_SUSPEND        = 0x18,
    BMI160_CMD_MAG_SET_PMU_NORMAL         = 0x19,
    BMI160_CMD_MAG_SET_PMU_LOWPOWER       = 0x1A,

    /* System Commands */
    BMI160_CMD_SOFT_RESET                 = 0xB6, // Triggers a full device reset
    BMI160_CMD_FIFO_FLUSH                 = 0xB0, // Clears the FIFO buffer
    BMI160_CMD_INT_RESET                  = 0xB1, // Resets interrupt engine
    BMI160_CMD_STEP_CNT_CLR               = 0xB2, // Resets the step counter
    
    /* Calibration and Memory Commands */
    BMI160_CMD_START_FOC                  = 0x03, // Start Fast Offset Compensation
    BMI160_CMD_PROG_NVM                   = 0xA0  // Triggers programming of the NVM
} bmi160_cmd_t;
