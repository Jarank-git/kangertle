#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "main";

static void servo_task(void *arg)
{
    while(1) {
        ESP_LOGI(TAG, "Servo Task is alive");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static int us_to_ticks(int us) {
    return us * 4096 / 20000;
}

void app_main(void)
{
    int pulse_us = 1500;
    int ticks = us_to_ticks(pulse_us);
    ESP_LOGI(TAG, "Pulse is %d us and ticks are %d", pulse_us, ticks);
    if (xTaskCreatePinnedToCore(servo_task, "servo", 4096, NULL, 5, NULL, 1) != pdPASS) {
        ESP_LOGE(TAG, "FAILED");
    }
    
}
