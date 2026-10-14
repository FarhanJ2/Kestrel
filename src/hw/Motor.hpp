#pragma once

#include <ctre/phoenix6/CANBus.hpp>
#include <ctre/phoenix6/TalonFX.hpp>

using namespace ctre::phoenix6;

// TalonFX wrapper: configuration and current limits, applying a Command
// (including FeedEnable), and reading telemetry back as a MotorState.

class Motor {
    private:
};