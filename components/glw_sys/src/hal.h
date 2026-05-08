#pragma once

#include "driver/gpio.h"
#include "esp_err.h"
#include "hal/gpio_types.h"

typedef gpio_num_t pin_t;

// ======= DIGITAL =========

typedef int level_t;

typedef enum {
    OUTPUT = GPIO_MODE_OUTPUT,
    INPUT = GPIO_MODE_INPUT
} pin_mode;

static inline void set_pin_mode(pin_t pin, pin_mode mode) {
    ESP_ERROR_CHECK(gpio_set_direction(pin, mode));
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
