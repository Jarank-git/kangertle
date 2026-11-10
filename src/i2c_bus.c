#include "i2c_bus.h"
#include "esp_log.h"
#include "config.h"

static const char *TAG = "i2c_bus";

esp_err_t i2c_bus_init(i2c_master_bus_handle_t *out_bus)
{
    // using ESP-IDF pre built struct for creating an I2C connection
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = -1,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&bus_cfg, out_bus);
    if (err != ESP_OK) return err;
    ESP_LOGI(TAG, "I2C bus ready");

    // scan: log every address that ACKs
    for (int addr = 0x08; addr < 0x78; addr++) {
        if (i2c_master_probe(*out_bus, addr, 50) == ESP_OK) {
            ESP_LOGI(TAG, "Device at 0x%02X", addr);
        }
    }
    return ESP_OK;
}
