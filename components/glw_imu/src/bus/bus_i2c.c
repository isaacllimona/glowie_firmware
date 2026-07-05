#include <stdint.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

#include "config.h"
#include "glw_imu/errors.h"
#include "platform.h"

// implements
#include "bus.h"

static i2c_master_dev_handle_t device_handle;

static void tie_pin_level(gpio_num_t pin, int level) {
    ESP_ERROR_CHECK(gpio_set_direction(pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(pin, level));
}

glw_imu_error_t glw_imu_bus_init(void) {
    tie_pin_level(GLW_IMU_SA0_PIN, GLW_IMU_SA0_LEVEL);
#ifdef GLW_IMU_SET_CS_HIGH
    tie_pin_level(GLW_IMU_CS_PIN, 1); // Tie CS high to enable I2C
#endif

    i2c_master_bus_config_t i2c_config = {
        .i2c_port = -1, // auto selection
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = GLW_IMU_SCL_PIN,
        .sda_io_num = GLW_IMU_SDA_PIN,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;
    PROPAGATE_ERROR_AS(i2c_new_master_bus(&i2c_config, &bus_handle), GLW_IMU_BUS_ERROR);

    i2c_device_config_t device_config = {.dev_addr_length = I2C_ADDR_BIT_LEN_7,
                                         .device_address = GLW_IMU_I2C_ADDRESS,
                                         .scl_speed_hz = GLW_IMU_I2C_SPEED_HZ};
    PROPAGATE_ERROR_AS(i2c_master_bus_add_device(bus_handle, &device_config, &device_handle),
                       GLW_IMU_BUS_ERROR);

    return GLW_IMU_OK;
}

glw_imu_error_t glw_imu_bus_write(uint8_t addr, const uint8_t *buffer, uint16_t len) {
    i2c_master_transmit_multi_buffer_info_t multibuffer[2] = {{.write_buffer = &addr, .buffer_size = 1},
                                                              {.write_buffer = buffer, .buffer_size = len}};

    PROPAGATE_ERROR_AS(i2c_master_multi_buffer_transmit(device_handle, multibuffer, 2, -1),
                       GLW_IMU_BUS_ERROR);

    return GLW_IMU_OK;
}

glw_imu_error_t glw_imu_bus_read(uint8_t addr, uint8_t *buffer, uint16_t len) {
    PROPAGATE_ERROR_AS(i2c_master_transmit_receive(device_handle, &addr, 1, // write register address
                                                   buffer, len,             // read data into buffer
                                                   -1),
                       GLW_IMU_BUS_ERROR);

    return GLW_IMU_OK;
}
