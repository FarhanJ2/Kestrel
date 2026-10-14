#pragma once

#include "Config.hpp"
#include "core/Types.hpp"
#include <ctre/phoenix/StatusCodes.h>
#include <ctre/phoenix6/CANBus.hpp>
#include <ctre/phoenix6/StatusSignal.hpp>
#include <ctre/phoenix6/TalonFX.hpp>
#include <ctre/phoenix6/controls/DutyCycleOut.hpp>
#include <ctre/phoenix6/controls/VelocityTorqueCurrentFOC.hpp>
#include <units/angular_jerk.h>
#include <units/current.h>
#include <units/temperature.h>
#include <units/voltage.h>

// TalonFX wrapper: configuration and current limits, applying a Command
// (including FeedEnable), and reading telemetry back as a MotorState.

class Motor {
    private:
        static constexpr ctre::phoenix6::CANBus CANBUS{Config::CANBUS_NAME};
        ctre::phoenix6::hardware::TalonFX motor{Config::_ID, CANBUS};

        // motion requests
        ctre::phoenix6::controls::DutyCycleOut dutyCycle;
        ctre::phoenix6::controls::VelocityTorqueCurrentFOC torqueCurrentFOC;

        // status signals
        ctre::phoenix6::StatusSignal<units::turns_per_second_t>& m_velocity_rot_per_sec = motor.GetVelocity(false);
        ctre::phoenix6::StatusSignal<units::ampere_t>& m_statorCurrent = motor.GetTorqueCurrent(false);
        ctre::phoenix6::StatusSignal<units::ampere_t>& m_supplyCurrent = motor.GetSupplyCurrent(false);
        ctre::phoenix6::StatusSignal<units::volt_t>& m_supplyVoltage = motor.GetSupplyVoltage(false);
        ctre::phoenix6::StatusSignal<units::celsius_t>& m_temperature = motor.GetDeviceTemp(false);
        bool m_connected = false;

        void SetVelocity(double wanted_vel_rad_per_sec);
        void SetDutyCycle(double output);

    public:
        Motor();
        ~Motor();
        void Feed(const Command& _cmd);
        MotorState Read();
};