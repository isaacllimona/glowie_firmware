#pragma once

#include <stdint.h>
#include <sys/types.h>

#include "bmi160_ll.h"

/**
 * @brief Initialise the connection to the bmi160 sensor and perform basic checks
**/
int bmi160_init(void);
