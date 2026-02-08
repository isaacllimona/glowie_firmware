#pragma once

#include "host/ble_uuid.h"

/* Defines a custom UUID as: 0000XXXX-1fbd-0481-2a47-f3b657518ec1 */
#define CUSTOM_UUID(uuid16) BLE_UUID128_DECLARE(0xc1, 0x8e, 0x51, 0x57, 0xb6, 0xf3, 0x47, 0x2a, 0x81, 0x04, 0xbd, 0x1f, (uuid16 & 0xFF), ((uuid16 >> 8) & 0xFF), 0x00, 0x00)


// ** Services **
#define UUID_SERVICE_CONTROL    CUSTOM_UUID(0x0000)
#define UUID_SERVICE_SCRIPT     CUSTOM_UUID(0x0100)

// ** Characteristics **
#define UUID_CHAR_ENABLE        CUSTOM_UUID(0x0001)
#define UUID_CHAR_BRIGHTNESS    CUSTOM_UUID(0x0002)
#define UUID_CHAR_RGB           CUSTOM_UUID(0x0003)

#define UUID_CHAR_SCRIPT_IN     CUSTOM_UUID(0x0101)
