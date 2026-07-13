#pragma once

#include "host/ble_uuid.h"

/* Defines a custom UUID as: 0000XXXX-1fbd-0481-2a47-f3b657518ec1 */
#define CUSTOM_UUID(uuid16) BLE_UUID128_DECLARE(0xc1, 0x8e, 0x51, 0x57, 0xb6, 0xf3, 0x47, 0x2a, 0x81, 0x04, 0xbd, 0x1f, (uuid16 & 0xFF), ((uuid16 >> 8) & 0xFF), 0x00, 0x00)


// ** Services **
#define UUID_SERVICE_CONTROL    CUSTOM_UUID(0x0000)

// ** Characteristics **
#define UUID_CHAR_COMMAND       CUSTOM_UUID(0x0001)
#define UUID_CHAR_CMD_OUT       CUSTOM_UUID(0x0002)
