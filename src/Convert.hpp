#pragma once

#include <numbers>

namespace conv {
    constexpr double kMphToMps = 0.44704;

    constexpr double MphToMps(double _mph) {
        return _mph * kMphToMps;
    }

    constexpr double MotorRadPerSecToRoadMps(double motor_rad_per_sec, double gear_ratio, double roller_radius_m) {
        return motor_rad_per_sec / (gear_ratio * roller_radius_m);
    }
}