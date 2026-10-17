#pragma once

#undef MIN
#define MIN(a, b) (a < b ? a : b)

#define CONFIG_DEVICE_NAME "glowie"
// #define CONFIG_USE_RANDOM_ADDRESS

#define CONFIG_COMMS_BLE_MTU 517

// Hold the MTU minus 3 bytes of ATT header, but no more than 512, which is the maxium attribute value length
// plus 1 for the string termination character
#define CONFIG_COMMS_COMMAND_BUFFER_SIZE (MIN(CONFIG_COMMS_BLE_MTU - 3, 512) + 1)
