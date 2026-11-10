#include "pca9685.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"

static const char *TAG = "pca9685";

#define PCA_REG_MODE1 0x00
#define PCA_REG_PRESCALE 0xFE
#define PCA_REG_LED0_ON_L 0x06

//helper fcn to define what goes where within the PCA9685
static esp_err_t pca_write(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t value) {
    //array of 2 bytes that holds which register to go to and what to put in there
    uint8_t buf[2] = {reg,value};

    //send over the data, and then add time until give up,  returns driver error code
    return i2c_master_transmit(dev, buf, 2, 100);
}

esp_err_t pca9685_init(i2c_master_bus_handle_t bus, i2c_master_dev_handle_t *out_dev)
{
    // quick probe to see if PCA9685 is found
    if (i2c_master_probe(bus, ADDR_PCA9685, 100) == ESP_OK) {
        ESP_LOGI(TAG, "FOUND at 0x%02X", ADDR_PCA9685);
    } else {
        ESP_LOGE(TAG, "PCA9685 not found");
    }

    // configuring the pca via esp-idf i2c device config struct
    i2c_device_config_t pca_cfg = {
        .dev_addr_length =  I2C_ADDR_BIT_LEN_7,
        .device_address = ADDR_PCA9685,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    esp_err_t err = i2c_master_bus_add_device(bus, &pca_cfg, out_dev);
    if (err != ESP_OK) return err;
    ESP_LOGI(TAG, "PCA9685 Added to Bus");

    // 0x10 is sleep for PCA9685, and you can only change prescale if in sleep
    err = pca_write(*out_dev, PCA_REG_MODE1, 0x10);
    if (err != ESP_OK) return err;

    // setting pre scale
    err = pca_write(*out_dev, PCA_REG_PRESCALE, 121);
    if (err != ESP_OK) return err;
    ESP_LOGI(TAG, "Set PRESCALE TO 121 (50hz)");

    // waking up the PCA9685 (0x20 = awake + auto-increment), oscillator needs >= 500 us
    err = pca_write(*out_dev, PCA_REG_MODE1, 0x20);
    if (err != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(2));
    ESP_LOGI(TAG, "PCA9685 is awake");
    return ESP_OK;
}

esp_err_t pca9685_set_pwm(i2c_master_dev_handle_t dev, uint8_t channel, uint16_t on, uint16_t off) {
    // row of 5 bytes, which box to start, two bytes for on and off, which gives value range from 0 - 4095
    uint8_t buf[5] = {
        PCA_REG_LED0_ON_L + 4 * channel,
        on & 0xFF, on >> 8,
        off & 0xFF, off >> 8,
    };
    return i2c_master_transmit(dev,buf,5,100);
}
