#pragma once

#include <stdint.h>

#include "glw_comms_ble.h"

typedef struct gatt_init_t {
    command_cb_t command_cb;
} gatt_init_t;

int gatt_init(gatt_init_t init);

void gatt_send_cmd_err(uint16_t conn_handle, const char *prefix, const char *output);
