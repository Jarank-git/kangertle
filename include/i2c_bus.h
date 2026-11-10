#pragma once
// Shared I2C bus (PCA9685 now, GY-87 later). Once more than one task uses the bus,
// the mutex that guards it belongs in this module.

#include "esp_err.h"
#include "driver/i2c_master.h"

// create the I2C master bus on PIN_I2C_SDA / PIN_I2C_SCL and log every device that answers
esp_err_t i2c_bus_init(i2c_master_bus_handle_t *out_bus);
