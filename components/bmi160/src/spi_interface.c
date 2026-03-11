#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "hal/spi_types.h"
#include "sdkconfig.h"

#include "bmi160_ll.h"

#define SPI_HOST SPI2_HOST

#define SPI_MISO_PIN CONFIG_BMI160_SPI_MISO_PIN
#define SPI_MOSI_PIN CONFIG_BMI160_SPI_MOSI_PIN
#define SPI_SCLK_PIN CONFIG_BMI160_SPI_SCLK_PIN
#define SPI_CS_PIN   CONFIG_BMI160_SPI_CS_PIN
#define SPI_CLOCK_HZ CONFIG_BMI160_SPI_CLOCK_HZ

static spi_device_handle_t device_handle;

void bmi160_interface_init(void) {
    spi_bus_config_t bus_config = {
        .miso_io_num = SPI_MISO_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .sclk_io_num = SPI_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    esp_err_t ret = spi_bus_initialize(SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    spi_device_interface_config_t device_config = {
        .command_bits = 1,
        .address_bits = 7,
        .clock_speed_hz = SPI_CLOCK_HZ,
        .spics_io_num = SPI_CS_PIN,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(SPI_HOST, &device_config, &device_handle);
    ESP_ERROR_CHECK(ret);

    // Dummy read to create a raising edge in CS, which selects SPI interface in the bmi160
    bmi160_read(0x7F);
}

void bmi160_interface_start(void) {
    // Dummy read to create a raising edge in CS, which selects SPI interface in the bmi160
    bmi160_read(0x7F);
}

enum {
    BMI160_SPI_CMD_WRITE = 0,
    BMI160_SPI_CMD_READ = 1,
};

void bmi160_write(bmi160_reg_t addr, uint8_t data) {
    spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_TXDATA,
        .cmd = BMI160_SPI_CMD_WRITE,
        .addr = addr,
        .length = 8,
        .tx_data = {data},
    };

    esp_err_t ret = spi_device_transmit(device_handle, &transaction);
    ESP_ERROR_CHECK(ret);
}

uint8_t bmi160_read(bmi160_reg_t addr) {
    spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_RXDATA,
        .cmd = BMI160_SPI_CMD_READ,
        .addr = addr,
        .length = 8,
    };

    esp_err_t ret = spi_device_transmit(device_handle, &transaction);
    ESP_ERROR_CHECK(ret);

    return transaction.rx_data[0];
}

void bmi160_read_multiple(bmi160_reg_t addr, unsigned int length, void *buffer) {
    spi_transaction_t transaction = {
        .cmd = BMI160_SPI_CMD_READ,
        .addr = addr,
        .length = length * 8,
        .rx_buffer = buffer,
    };

    esp_err_t ret = spi_device_transmit(device_handle, &transaction);
    ESP_ERROR_CHECK(ret);
}
