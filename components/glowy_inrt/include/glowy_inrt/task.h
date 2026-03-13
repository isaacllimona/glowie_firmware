#pragma once

#include <stdbool.h>

/**
 * Start a task for the inertial state estimation loop
 *
 * Requires prior initialisation of the module by running glowy_inrt_init()
 */
bool glowy_inrt_task_start(unsigned int priority);
