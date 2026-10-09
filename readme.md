# glowie_firmware

Firmware for LED juggling and visual props.

Main features:
- An embedded Lua interpreter to program light patterns. Scripts can be uploaded remotely via BLE.
- Inertial state estimation (throw detection, velocity and height estimation) for interactive patterns. (WIP)
- BLE remote control.
- Power and battery management.

## Hardware

- **Microcontroller**: glowie_firmware is being developed for the ESP32-C3 microcontroller, though it should work on other ESP32 variants.
The project is designed to be easily ported to other microcontrollers, with most modules being platform-independent or keeping platform dependencies in a thin abstraction layer.

- **LEDs**: The firmware currently supports WS2812 LEDs. Low-level LED control is abstracted in a small driver to allow easy implementation of other controllers.

- **IMU**: The IMU driver is implemented for the LSM6DSV32X over an I2C bus.

## Building and flashing

ESP-IDF v6.0 or later is required.

Build and flash using the idf tools:
```
idf.py set-target esp32c3
idf.py flash
```

## Development

### Code structure

The project is divided into independent modules, with few direct dependencies between them. Application code puts together the necessary modules and injects all required dependencies.

#### Modules

There are two base modules most other modules depend on:

- **glw_log**: Logging system.

- **glw_os**: OS functionality, mostly a wrapper over FreeRTOS.

The rest of the modules are mostly independent from each other:

- **glw_script**: Provides an embedded Lua interpreter and defines custom Lua libraries to interact with the device.

- **glw_comms**: Implements communication with the remote controller over BLE using the Nimble stack. The interface is kept generic to support different communication methods in the future.

- **glw_command**: Simple parser for remote commands.

- **glw_sys**: Implements power modes, charge control and monitoring, and button press detection.

- **glw_led**: Controls the LEDs, allowing setting the color of all or individual LEDs with a configurable mapping from pixel index to physical LED. Also supports setting a global brightness modifier, and enabling or disabling all LEDs.

- **glw_imu**: Driver for the IMU, exposing a device-agnostic interface.

- **glw_inrt**: Inertial state estimation.

#### Configuration

Configuration values are currently scattered in config headers inside each module.

### Build system

This project uses a CMake-based build system. For ESP32 builds, a wrapper around build system v2 from ESP-IDF is used to create cross-compilation and flashing targets and to add ESP-IDF components.

### Testing

Two kinds of tests are implemented:
- Native unit tests. These can be run with the `run_native_tests.sh` script
- On-device tests, which test the integration of single modules with the hardware. On-device tests for all modules are run together from a single test app, which can be flashed with the *test_app-flash* target.
