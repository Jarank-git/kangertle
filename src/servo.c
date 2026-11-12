#include "servo.h"
#include "ik.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "pca9685.h"


static const char *TAG = "servo";

// handle saved by servo_init, used by every function in this file
static i2c_master_dev_handle_t s_pca;

// per-channel trim in microseconds, added to the 1500 us centre (index = PCA channel)
static const int trim_us[16] = {
    /* ch0 */ -60, /* ch1 */ 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

// per-channel direction: +1 if +us already matches the convention, -1 if the servo is mirrored
// convention: shoulder + = leg outward, femur + = foot forward, linkage + = foot up
// unused channels (3, 7, 11, 12) stay 0
static const int dir_sign[16] = {
    [CH_FR_SHOULDER] = -1, [CH_FR_FEMUR] = +1, [CH_FR_LINKAGE] = -1,
    [CH_FL_SHOULDER] = +1, [CH_FL_FEMUR] = -1, [CH_FL_LINKAGE] = -1,
    [CH_RR_SHOULDER] = +1, [CH_RR_FEMUR] = +1, [CH_RR_LINKAGE] = +1,
    [CH_RL_SHOULDER] = -1, [CH_RL_FEMUR] = -1, [CH_RL_LINKAGE] = -1,
};

// helper function to ensure ticks are calculated correctly, returns wether it worked or not, and updates the ticks value  via the address in the stack
static esp_err_t us_to_ticks(int us, int *ticks) {
    if (us < 500 || us > 2500) {
        return ESP_ERR_INVALID_ARG;
    }

    *ticks = us * 4096/20000;
    return ESP_OK;
}

void servo_init(i2c_master_dev_handle_t pca)
{
    s_pca = pca;
}

esp_err_t servo_set_us(int ch, int us)
{
    int ticks;
    esp_err_t err = us_to_ticks(us, &ticks);
    if (err != ESP_OK) return err;
    return pca9685_set_pwm(s_pca, ch, 0, ticks);
}

esp_err_t servo_center_all(void)
{
    //setting up final convertion from us to ticks, then using the pca to move the actual servo
    for (int i = 0; i < 16; i++) {
        esp_err_t err = servo_set_us(i, 1500 + trim_us[i]);
        if (err != ESP_OK) return err;
        ESP_LOGI(TAG, "ch %d: %d us", i, 1500 + trim_us[i]);
    }
    return ESP_OK;
}

//IK uses degrees, so we want to convert from degrees to us
 esp_err_t joint_set_deg(int ch, float deg)
  {
// reject channels that don't exist or have no joint (dir_sign0 = unused)
      if (ch < 0 || ch > 15 || dir_sign[ch] == 0) {
          return ESP_ERR_INVALID_ARG;
      }

    //convert from degree to us
      float us_per_deg = 2000.0f / 180.0f;
      //apply specfic trim for the respective servo and the direction flip
      int us = 1500 + trim_us[ch] + (int)(dir_sign[ch] * deg *
    us_per_deg);

    //moves the servo, then returns ESP_OK or error code
      return servo_set_us(ch, us);
  }


// used for calibration, and figuring out channels and direction conventions
void servo_direction_test(void)
{
    while (1) {
        for (int ch = 0; ch < 16; ch++) {
            ESP_LOGI(TAG, "testing ch %d: +100 us", ch);
            ESP_ERROR_CHECK(servo_set_us(ch, 1500 + trim_us[ch] + dir_sign[ch] * 100));
            vTaskDelay(pdMS_TO_TICKS(5000));
            ESP_ERROR_CHECK(servo_set_us(ch, 1500 + trim_us[ch]));
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}
