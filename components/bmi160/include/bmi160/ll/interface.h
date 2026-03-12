#pragma once

#include <stdint.h>

#include "registers.h"

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
