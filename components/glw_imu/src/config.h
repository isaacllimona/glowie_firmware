#pragma once

// PINS
#define GLW_IMU_SCL_PIN 6
#define GLW_IMU_SDA_PIN 5
#define GLW_IMU_SA0_PIN 10
#define GLW_IMU_CS_PIN  7

#define GLW_IMU_SET_CS_HIGH  // Tie the CS pin HIGH
#define GLW_IMU_SA0_LEVEL 0  // Digital level (0/1) to tie the SA0 pin to

#define GLW_IMU_I2C_ADDRESS  0x6AU
#define GLW_IMU_I2C_SPEED_HZ 400000 // I2C fast mode (400kHz)
