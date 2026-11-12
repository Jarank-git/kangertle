#include "ik.h"
#include <math.h>

esp_err_t ik_leg_2d(float x_mm, float z_mm, float *femur_deg, float *linkage_deg)
{
    //straight line distance from femur to the foot
    float d = sqrtf(x_mm * x_mm + z_mm * z_mm);

    //triangle only exists if the foot is within reach of pythagoras
    if (d > LEG_FEMUR_MM + LEG_TIBIA_MM || d < 1.0f) {
        return ESP_ERR_INVALID_ARG;
    }

    //knee angle between femur and lower leg (cosine law) 180 = straight leg
    float cos_knee = (LEG_FEMUR_MM * LEG_FEMUR_MM + LEG_TIBIA_MM * LEG_TIBIA_MM - d * d) / (2.0f * LEG_FEMUR_MM * LEG_TIBIA_MM);
    float knee_deg = acosf(cos_knee) * 180.0f / (float)M_PI;

    float alpha_deg = atan2f(x_mm, z_mm) * 180.0f / (float)M_PI;

    float cos_beta = (LEG_FEMUR_MM * LEG_FEMUR_MM + d * d - LEG_TIBIA_MM * LEG_TIBIA_MM) / (2.0f *
    LEG_FEMUR_MM * d);
    float beta_deg = acosf(cos_beta) * 180.0f / (float)M_PI;


    //femur angle from the vertical accounting for the zero position of the servo at 1500 us
    *femur_deg = (alpha_deg - beta_deg) - LEG_FEMUR_ZERO_DEG;
    //takes the lower leg from vertical, minus where it sits at servo center (1500us), would be different if you mount legs differently
    *linkage_deg = (alpha_deg + beta_deg) - LEG_TIBIA_ZERO_DEG;
    return ESP_OK;
}
