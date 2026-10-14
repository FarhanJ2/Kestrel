#pragma once

#include <iostream>

namespace Config {
    // SocketCAN Interface ID: Insert when physical device is here; for CANivore just put *
    inline constexpr std::string_view CANBUS_NAME = "*";
    inline constexpr uint32_t _ID = 0;

    // motor config
    inline constexpr uint32_t STATOR_CURRENT_LIMIT_AMPS = 40;
    inline constexpr bool STATOR_LIMIT_ENABLED = true;
    inline constexpr uint32_t SUPPLY_CURRENT_LIMIT_AMPS = 25;
    inline constexpr bool SUPPLY_LIMIT_ENABLED = true;

    inline constexpr double RAMP_SECONDS = 0.25;
    inline constexpr uint32_t STATUS_SIGNAL_FREQ = 100;

    inline constexpr uint32_t TIMEOUT_MS = 100;
}