#pragma once

/** 
 * Initialize the module 
 * 
 * Initialises the IMU, state estimation loop, and data dispatcher
 */
void glowy_inrt_init(void);

/** Run the inertial state estimation loop */
void glowy_inrt_run(void);
