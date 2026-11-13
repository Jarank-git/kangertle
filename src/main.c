#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include "pca9685.h"
#include "servo.h"
#include "ik.h"
#include <driver/uart.h>
#include <stdio.h>

static const char *TAG = "main";

// servo task, used to setup i2c and PCA9685 to be used for balancing algorithim and zeroing
static void servo_task(void *arg)
{
    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_bus_init(&bus));

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0));

    i2c_master_dev_handle_t pca;
    ESP_ERROR_CHECK(pca9685_init(bus, &pca));

    servo_init(pca);
    ESP_ERROR_CHECK(servo_center_all());
    ESP_LOGI(TAG, "all joints at trimmed centre");

    float femur, linkage;
    esp_err_t err = ik_leg_2d(49.8f, 211.7f, &femur, &linkage);
    ESP_LOGI(TAG, "IK Test: %s femur = %.1f linkage=%.1f", esp_err_to_name(err), femur, linkage);

    char line[32];
    int len = 0;
     while(1) {
        uint8_t c;
        //set up UART, if a value is sent over UART then enter the if statement
        if (uart_read_bytes(UART_NUM_0, &c, 1, portMAX_DELAY) == 1) {
            //if '\n' is detected, we end line and set the final value to '\0' (ending the line)
            if (c == '\n') {
                line[len] = '\0';
                int ch;
                float deg;
                if (sscanf(line, "%d %f", &ch, &deg) == 2) { 
                    ESP_LOGI(TAG, "ch=%d deg=%.1f" ch, deg);
                } else  {
                    ESP_LOGW(TAG, "usage: <ch> <deg> e.g 0 10");
                }
                len = 0;
            } else if (len <sizeof(line) - 1) {
                line[len++] = c;
            }
        }
     }
}

// main
void app_main(void)
{
    //creating a task on core 1, setting up PCA9685 and doing communication
    if (xTaskCreatePinnedToCore(servo_task, "servo", 4096, NULL, 5, NULL, 1) != pdPASS) {
        ESP_LOGE(TAG, "FAILED");
    }
}
