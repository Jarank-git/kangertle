#pragma once
// Central pin / bus configuration. Change wiring here, not scattered through the code.
// Plain C has no constexpr (that's C++), so #define is the normal way to name constants.
// Wrap values in parentheses or add a U suffix so they behave predictably inside expressions.

// Shared I2C bus: PCA9685 + GY-87 (MPU6050, HMC5883L/QMC5883L, BMP180).
// GPIO8/9 are not strapping, USB, flash or PSRAM pins on the ESP32-S3.
#define PIN_I2C_SDA 8
#define PIN_I2C_SCL 9

// 400 kHz "fast mode". Drop to 100000 if you see errors with long wires.
#define I2C_FREQ_HZ 400000U

// Expected I2C addresses (7-bit)
#define ADDR_PCA9685         0x40  // A0-A5 jumpers all open
#define ADDR_PCA9685_ALLCALL 0x70  // PCA9685 all-call address, also answers
#define ADDR_MPU6050         0x68  // AD0 low (0x69 if AD0 high)
#define ADDR_HMC5883L        0x1E  // only visible after enabling MPU6050 I2C bypass
#define ADDR_QMC5883L        0x0D  // clone magnetometer on many GY-87 boards
#define ADDR_BMP180          0x77
