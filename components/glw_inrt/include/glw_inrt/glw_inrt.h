#pragma once

/** 
 * Initialize the module 
 * 
 * Initialises the IMU, state estimation loop, and data dispatcher
 */
void glw_inrt_init(void);

/** Run the inertial state estimation loop */
void glw_inrt_run(void);
