# Kangertle

Kangertle is a 12-servo quadruped robot dog. It combines a SpotMicro-style frame with the Kangal leg design, controlled by an ESP32-S3 running FreeRTOS firmware written in C on ESP-IDF.

## About

The project is a hands-on way to learn embedded firmware, FreeRTOS and legged robot control from the ground up. The firmware does not use Arduino or third-party driver libraries: the drivers (starting with the PCA9685 servo controller) are written from the datasheets.

The robot carries its own electronics and battery. The ESP32-S3 handles low-level control on board, and a PC connects over WiFi for high-level commands and telemetry.

## Hardware

| Part | Details |
|---|---|
| Controller | ESP32-S3-WROOM-1 N16R8 devkit |
| Servos | 12x DS3218 (180 degree), 3 per leg: shoulder, femur, knee linkage |
| Servo driver | PCA9685 16-channel PWM board over I2C |
| IMU | GY-87 (MPU6050, magnetometer, BMP180) over I2C |
| Power | 2S LiPo, 6 V 20 A UBEC for the servo rail |

## Repository layout

| Path | Contents |
|---|---|
| `src/` | Firmware source (ESP-IDF, C) |
| `include/config.h` | Pin map, I2C addresses and servo channel map |
| `CAD/` | Frame, legs, shell and mounting plate (Fusion 360 and STEP) |
| `platformio.ini`, `sdkconfig.defaults` | Build and chip configuration |

Build and flash with PlatformIO: `pio run -t upload -t monitor`.

## Goals

- **CAD:** modify the SpotMicro frame to work with the Kangal leg design.
- **Calibration software:** per-joint trim and direction so every leg responds the same way to the same command.
- **Inverse kinematics and gait:** compute joint angles from foot positions, then coordinate the legs into a walking gait.
- **GUI:** a PC application for control, calibration and live telemetry over WiFi.
- **Machine learning walking model:** train a walking policy in simulation and run it on the robot.

## Status

- Done: mechanical build, power wiring, ESP-IDF firmware setup, PCA9685 driver, servo zeroing.
- Done: calibration of all 12 joints (channel map, trim, direction).
- In progress: joint control in degrees, leading into inverse kinematics.
- Planned: gait, WiFi link and GUI, IMU readout and balance control, machine learning walking model.

## Credits

- **SpotMicro:** the original open-source quadruped this frame is based on.
- **Kangal:** the leg design used on this robot.
