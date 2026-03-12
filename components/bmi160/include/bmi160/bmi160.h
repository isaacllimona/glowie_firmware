#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ll/datatypes.h"

/**
 * @brief Initialise the connection to the bmi160 sensor and perform basic checks
 **/
int bmi160_init(void);

void bmi160_soft_reset(void);

void bmi160_acc_enable(bmi160_acc_config_t config, bmi160_acc_range_t range);
void bmi160_gyro_enable(bmi160_gyro_config_t config, bmi160_gyro_range_t range);
