#pragma once
// Leg inverse kinematics: foot position -> joint angles (degrees, in the direction convention).

#include "esp_err.h"

#define LEG_FEMUR_MM 130.0f  // femur, pivot to pivot (Yertle URDF, confirmed by measuring)
#define LEG_TIBIA_MM 130.0f  // lower leg, knee pivot to foot tip (measured)

// 2D IK in the leg's side plane: foot at x mm forward and z mm below the femur pivot
esp_err_t ik_leg_2d(float x_mm, float z_mm, float *femur_deg, float *linkage_deg);
