// Step 1 bring-up: I2C bus scanner running as a FreeRTOS task.
// Goal: prove the ESP32 <-> PCA9685 / GY-87 wiring before writing any driver code.

#include <Arduino.h>
#include <Wire.h>

#include "config.h"

static const char *deviceName(uint8_t addr) {
  switch (addr) {
    case ADDR_PCA9685:         return "PCA9685 servo driver";
    case ADDR_PCA9685_ALLCALL: return "PCA9685 all-call (normal, ignore)";
    case ADDR_MPU6050:         return "MPU6050 accel/gyro";
    case 0x69:                 return "MPU6050 (AD0 high)";
    case ADDR_HMC5883L:        return "HMC5883L magnetometer";
    case ADDR_QMC5883L:        return "QMC5883L magnetometer (clone)";
    case ADDR_BMP180:          return "BMP180 barometer";
    default:                   return "unknown";
  }
}

// Writing 1 to MPU6050 register 0x37 (INT_PIN_CFG) bit 1 (I2C_BYPASS_EN) connects the
// GY-87's magnetometer straight onto our bus. Without this it hides behind the MPU6050.
// Also clear sleep mode (PWR_MGMT_1 = 0x6B) so the chip responds normally.
static void enableMpuBypass() {
  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(ADDR_MPU6050);
  Wire.write(0x37);
  Wire.write(0x02);
  Wire.endTransmission();
}

// A FreeRTOS task is just a function that never returns. The scheduler time-slices it
// with other tasks; vTaskDelay() blocks it so lower-priority tasks (and the idle task,
// which feeds the watchdog) get CPU time.
static void i2cScanTask(void *) {
  for (;;) {
    enableMpuBypass();

    Serial.println("\n--- I2C scan ---");
    int found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) {  // 0 = device ACKed its address
        Serial.printf("  0x%02X  %s\n", addr, deviceName(addr));
        found++;
      }
    }
    Serial.printf("%d device(s) found\n", found);
    if (found == 0) {
      Serial.println("  Nothing: check SDA/SCL swap, 3.3V + GND to modules, common ground.");
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup() {
  Serial.begin(115200);
  // Native USB-CDC re-enumerates on reset; give the PC a moment to reattach the
  // monitor so we don't miss the first lines.
  delay(2000);
  Serial.println("Yertle I2C bring-up");
  Serial.printf("PSRAM: %u bytes, flash: %u bytes\n", ESP.getPsramSize(), ESP.getFlashChipSize());

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, I2C_FREQ_HZ);

  // Stack is in BYTES on ESP32 (unlike vanilla FreeRTOS, which uses words).
  // Pin to core 1: core 0 runs the WiFi/Bluetooth stack, so keep app work off it.
  xTaskCreatePinnedToCore(i2cScanTask, "i2c_scan", 4096, nullptr, 1, nullptr, 1);
}

void loop() {
  // Arduino's loop() is itself just a FreeRTOS task. We don't need it, so delete it
  // to free its stack.
  vTaskDelete(nullptr);
}
