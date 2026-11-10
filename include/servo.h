#pragma once
// Joint layer: turns "joint + angle" into a PCA9685 pulse, applying per-channel trim and direction.
// Everything above this (IK, gait) works in degrees and the direction convention:
//   shoulder + = leg outward, femur + = foot forward, linkage + = foot up

#include "esp_err.h"
#include "driver/i2c_master.h"

// remember the PCA9685 handle that every servo call uses
void servo_init(i2c_master_dev_handle_t pca);

// set one channel to a raw pulse in microseconds (500-2500), no trim or direction applied
esp_err_t servo_set_us(int ch, int us);

// set every channel to its trimmed centre (1500 us + trim)
esp_err_t servo_center_all(void);

// set one joint to an angle in degrees (0 = centre, + = convention direction)
esp_err_t joint_set_deg(int ch, float deg);

// calibration check: moves each joint +100 us in its convention direction, 5 s per channel, forever
void servo_direction_test(void);
