#pragma once

#include <strings.h>

typedef enum {
    BLE_ERR_INSUFFICIENT_MEMORY = 0x80,
    BLE_ERR_RESOURCE_BUSY = 0x81,
    BLE_ERR_BAD_COMMAND = 0x84,
} glw_comms_ble_error_t;

typedef glw_comms_ble_error_t (*command_cb_t)(const char *input);

typedef struct {
    command_cb_t command_cb; //< get the received command as a null-terminated string.
} glw_comms_ble_init_t;

void glw_comms_ble_init(glw_comms_ble_init_t init);

void glw_comms_ble_stop(void);

void glw_comms_ble_send_cmd_err(const char *prefix, const char *msg);
