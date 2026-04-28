#pragma once

#include "glw_comms.h"

typedef struct gatt_init_t {
    glw_comms_callbacks_t callbacks;
} gatt_init_t;

int gatt_init(gatt_init_t init);
