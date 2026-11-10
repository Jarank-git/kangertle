#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include "pca9685.h"
#include "servo.h"

static const char *TAG = "main";

// servo task, used to setup i2c and PCA9685 to be used for balancing algorithim and zeroing
static void servo_task(void *arg)
{
    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_bus_init(&bus));

    i2c_master_dev_handle_t pca;
    ESP_ERROR_CHECK(pca9685_init(bus, &pca));

    servo_init(pca);
    ESP_ERROR_CHECK(servo_center_all());
    ESP_LOGI(TAG, "all joints at trimmed centre");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
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
