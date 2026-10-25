#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "config.h"

static const char *TAG = "main";
#define PCA_REG_MODE1 0x00
#define PCA_REG_PRESCALE 0xFE
#define PCA_REG_LED0_ON_L 0x06

// helper function to ensure ticks are calculated correctly, returns wether it worked or not, and updates the ticks value  via the address in the stack
static int us_to_ticks(int us, int *ticks) {
    if (us < 500 || us > 2500) {
        return ESP_ERR_INVALID_ARG;
    } 

    *ticks = us * 4096/20000;
    return ESP_OK;
}

// helper fcn to turn servo on for 0 - 307 ticks to center it
static esp_err_t pca_set_pwm(i2c_master_dev_handle_t dev, uint8_t channel, uint16_t on, uint16_t off) {
    // row of 5 bytes, which box to start, two bytes for on and off, which gives value range from 0 - 4095
    uint8_t buf[5] = {
        PCA_REG_LED0_ON_L + 4 * channel,
        on & 0xFF, on >> 8,
        off & 0xFF, off >> 8,
    };
    return i2c_master_transmit(dev,buf,5,100);
}

//helper fcn to define what goes where within the PCA9685
static esp_err_t pca_write(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t value) {
    //array of 2 bytes that holds which register to go to and what to put in there
    uint8_t buf[2] = {reg,value};

    //send over the data, and then add time until give up,  returns driver error code
    return i2c_master_transmit(dev, buf, 2, 100);
}



// servo task, used to setup i2c and PCA9685 to be used for balancing algorithim and zeroing
static void servo_task(void *arg)
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

    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));
    ESP_LOGI(TAG, "I2C bus ready");

    // quick probe to see if PCA9685 is found
    if (i2c_master_probe(bus, ADDR_PCA9685, 100) == ESP_OK) {
        ESP_LOGI(TAG, "FOUND at %d",ADDR_PCA9685);
    } else {
        ESP_LOGE(TAG, "PCA9675 not found");
    }

    // configuring the pca via esp-idf i2c device config struct
    i2c_device_config_t pca_cfg = {
        .dev_addr_length =  I2C_ADDR_BIT_LEN_7,
        .device_address = ADDR_PCA9685,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    // adds values into the pca config
    i2c_master_dev_handle_t pca;
    //make sure its setting up properly
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &pca_cfg, &pca));
    ESP_LOGI(TAG, "PCA9685 Added to Bus");
    // 0x10 is sleep for PCA9685, and you can only change prescale if in sleep
    ESP_ERROR_CHECK(pca_write(pca, PCA_REG_MODE1, 0x10));
    ESP_LOGI(TAG, "Wrote MODE1");

    // setting pre scale
    ESP_ERROR_CHECK(pca_write(pca, PCA_REG_PRESCALE, 121));
    ESP_LOGI(TAG, "Set PRESCALE TO 121 (50hz)");


    // waking up the PCA9685
    ESP_ERROR_CHECK(pca_write(pca,PCA_REG_MODE1, 0x20));
    vTaskDelay(pdMS_TO_TICKS(2));
    ESP_LOGI(TAG, "PCA9685 is awake");


    //setting up final convertion from us to ticks, then using the pca to move the actual servo
    int ticks;
    ESP_ERROR_CHECK(us_to_ticks(1500, &ticks));
    for (int i = 0; i < 16; i++ ) {
        ESP_ERROR_CHECK(pca_set_pwm(pca, i, 0 , ticks));
    }
    ESP_LOGI(TAG, "all 16 channels set to %d ticks (center)", ticks);

    while (1) {
          vTaskDelay(pdMS_TO_TICKS(1000));
      }
}


// main 
void app_main(void)
{
    int pulse_us = 1500;
    int ticks;
    // setup variable which holds the result of the us_to_ticks fcn
    esp_err_t err = us_to_ticks(pulse_us, &ticks);
    // if the error is not okay, then log an error, if not log the value
    if (err != ESP_OK){
        ESP_LOGE(TAG, "us_to_ticks failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Pulse is %d and ticks are %d", pulse_us, ticks);
    }
    
    //creating a task on core 1, setting up PCA9685 and doing communication
    if (xTaskCreatePinnedToCore(servo_task, "servo", 4096, NULL, 5, NULL, 1) != pdPASS) {
        ESP_LOGE(TAG, "FAILED");
    }
    
}
