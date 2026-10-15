#pragma once

#include <stdint.h>

void gap_start(void);

/**
 * Get the handle of the currently active connection
 * If no device is connected, return BLE_HS_CONN_HANDLE_NONE
 */
uint16_t gap_get_active_connection(void);

void gap_disconnect(void);
