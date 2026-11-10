#pragma once
// PCA9685 16-channel PWM driver, written from the datasheet.

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

// add the PCA9685 to the bus and set it up for 50 Hz: sleep -> prescale 121 -> wake + auto-increment
esp_err_t pca9685_init(i2c_master_bus_handle_t bus, i2c_master_dev_handle_t *out_dev);

// set one channel's ON and OFF points on the 0-4095 counter
esp_err_t pca9685_set_pwm(i2c_master_dev_handle_t dev, uint8_t channel, uint16_t on, uint16_t off);
