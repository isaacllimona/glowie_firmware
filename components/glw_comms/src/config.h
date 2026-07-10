#pragma once

#define CONFIG_DEVICE_NAME "glowie"
// #define CONFIG_USE_RANDOM_ADDRESS


#define CONFIG_COMMS_BLE_MTU 517

// Hold the MTU minus 3 bytes of ATT header
#define CONFIG_COMMS_COMMAND_BUFFER_SIZE (CONFIG_COMMS_BLE_MTU - 3)
