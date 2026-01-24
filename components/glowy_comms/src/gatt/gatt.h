#pragma once

#include "glowy_comms.h"

typedef struct gatt_init_t {
    comms_callbacks_t callbacks;
} gatt_init_t;

int gatt_init(gatt_init_t init);
