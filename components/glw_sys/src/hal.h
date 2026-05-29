#pragma once

#include <assert.h>

#include "driver/gpio.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_err.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"

typedef gpio_num_t pin_t;

// ======= DIGITAL =========

typedef int level_t;

typedef enum {
    OUTPUT = GPIO_MODE_OUTPUT,
    INPUT = GPIO_MODE_INPUT
} pin_mode;

static inline void set_pin_mode(pin_t pin, pin_mode mode) {
    ESP_ERROR_CHECK(gpio_set_direction(pin, (gpio_mode_t)mode));
}

static inline void enable_pullup(pin_t pin) {
    ESP_ERROR_CHECK(gpio_pullup_en(pin));
}

static inline void disable_pullup(pin_t pin) {
    ESP_ERROR_CHECK(gpio_pullup_dis(pin));
}

static inline void enable_pulldown(pin_t pin) {
    ESP_ERROR_CHECK(gpio_pulldown_en(pin));
}

static inline void disable_pulldown(pin_t pin) {
    ESP_ERROR_CHECK(gpio_pulldown_dis(pin));
}

static inline void set_level(pin_t pin, level_t level) {
    ESP_ERROR_CHECK(gpio_set_level(pin, level));
}

static inline level_t get_level(pin_t pin) {
    return gpio_get_level(pin);
}

// ====== ADC ======

typedef int adc_analog_val_t;

const static adc_atten_t _adc_atten = ADC_ATTEN_DB_0;
const static adc_bitwidth_t _adc_bitwidth = ADC_BITWIDTH_DEFAULT;

static adc_oneshot_unit_handle_t _adc_unit1_handle;

static inline void _adc_init(void) {
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &_adc_unit1_handle));
}

// ----

typedef struct {
   adc_channel_t channel;
   adc_cali_handle_t cali_handle;
} adc_t;

static inline void adc_init_pin(pin_t pin, adc_t *adc) {
    if(_adc_unit1_handle == NULL)
        _adc_init();

    adc_channel_t channel;
    adc_unit_t unit;
    adc_oneshot_io_to_channel(pin, &unit, &channel);
    assert(unit == ADC_UNIT_1);

    adc_oneshot_chan_cfg_t config = {
        .atten = _adc_atten,
        .bitwidth = _adc_bitwidth
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(_adc_unit1_handle, channel, &config));

    // Calibration
    adc_cali_handle_t cali_handle;
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = _adc_atten,
        .bitwidth = _adc_bitwidth
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));

    adc->channel = channel;
    adc->cali_handle = cali_handle;
}

static inline adc_analog_val_t adc_read(adc_t *adc) {
    int raw, val;
    ESP_ERROR_CHECK(adc_oneshot_read(_adc_unit1_handle, adc->channel, &raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc->cali_handle, raw, &val));
    return val;
}
