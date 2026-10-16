#pragma once
// Central pin / bus configuration. Change wiring here, not scattered through the code.

// Shared I2C bus: PCA9685 + GY-87 (MPU6050, HMC5883L/QMC5883L, BMP180).
// GPIO8/9 are the Arduino-ESP32 S3 defaults and are not strapping, USB, flash or PSRAM pins.
constexpr int PIN_I2C_SDA = 8;
constexpr int PIN_I2C_SCL = 9;

// 400 kHz "fast mode". All devices on the bus support it; drop to 100000 if you see
// NACKs/garbage with long wires.
constexpr uint32_t I2C_FREQ_HZ = 400000;

// Expected I2C addresses (7-bit)
constexpr uint8_t ADDR_PCA9685 = 0x40;  // A0-A5 jumpers all open
constexpr uint8_t ADDR_PCA9685_ALLCALL = 0x70;  // PCA9685 all-call address, also answers
constexpr uint8_t ADDR_MPU6050 = 0x68;  // AD0 low (0x69 if AD0 high)
constexpr uint8_t ADDR_HMC5883L = 0x1E;  // only visible after enabling MPU6050 I2C bypass
constexpr uint8_t ADDR_QMC5883L = 0x0D;  // clone magnetometer on many GY-87 boards
constexpr uint8_t ADDR_BMP180 = 0x77;
