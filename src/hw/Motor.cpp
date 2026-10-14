#include "hw/Motor.hpp"
#include "Config.hpp"
#include "Convert.hpp"
#include "core/Types.hpp"
#include <cstdlib>
#include <ctre/phoenix/StatusCodes.h>
#include <ctre/phoenix6/StatusSignal.hpp>
#include <ctre/phoenix6/core/CoreTalonFX.hpp>
#include <ctre/phoenix6/signals/SpnEnums.hpp>
#include <ctre/phoenix6/unmanaged/Unmanaged.hpp>
#include <numbers>
#include <stdexcept>
#include <units/frequency.h>

using namespace ctre::phoenix6;

Motor::Motor()
    : dutyCycle{0.0},
      torqueCurrentFOC{units::turns_per_second_t{0}} 
{
    ctre::phoenix6::configs::TalonFXConfiguration config{};
    config.CurrentLimits.StatorCurrentLimit = units::ampere_t{Config::STATOR_CURRENT_LIMIT_AMPS};
    config.CurrentLimits.StatorCurrentLimitEnable = Config::STATOR_LIMIT_ENABLED;
    config.CurrentLimits.SupplyCurrentLimit = units::ampere_t{Config::SUPPLY_CURRENT_LIMIT_AMPS};
    config.CurrentLimits.SupplyCurrentLimitEnable = Config::SUPPLY_LIMIT_ENABLED;

    config.MotorOutput.NeutralMode = signals::NeutralModeValue::Coast;
    config.MotorOutput.Inverted = signals::InvertedValue::CounterClockwise_Positive;
    config.OpenLoopRamps.DutyCycleOpenLoopRampPeriod = units::second_t{Config::RAMP_SECONDS};

    ctre::phoenix::StatusCode status = ctre::phoenix::StatusCode::StatusCodeNotInitialized;
    for (int i = 0; i < 5 && !status.IsOK(); ++i) {
        status = motor.GetConfigurator().Apply(config);
    }
    if (!status.IsOK()) {
        throw std::runtime_error("TALONFX config failed");
    }
    BaseStatusSignal::SetUpdateFrequencyForAll(units::hertz_t{Config::STATUS_SIGNAL_FREQ},
        m_velocity_rot_per_sec, m_statorCurrent, m_supplyCurrent, m_supplyVoltage, m_temperature);
    
}

Motor::~Motor() {
    motor.SetControl(controls::NeutralOut{});
}

void Motor::Feed(const Command &_cmd) {
    ctre::phoenix::unmanaged::FeedEnable(Config::TIMEOUT_MS);
    // command state is verified and checked for safety so no checking here just execute.
    switch (_cmd._mode) {
        case Mode::PID:
            // implement velocity torque current foc. it is literally perfect for this application
            break;
        
        case Mode::DUTY_CYCLE:
            motor.SetControl(dutyCycle.WithOutput(units::dimensionless::scalar_t{_cmd.duty_cycle}));
            break;

        case Mode::BRAKE:
            // apply a duty cycle to oppose motion find way to make smarter before implementing
            motor.SetControl(controls::StaticBrake{});
            break;

        case Mode::NEUTRAL:
        default:
            motor.SetControl(controls::NeutralOut{});
            break;
    }
}

MotorState Motor::Read() {
    BaseStatusSignal::RefreshAll(
        m_velocity_rot_per_sec, m_statorCurrent, m_supplyCurrent, m_supplyVoltage, m_temperature);
    MotorState state;
    state.connected = BaseStatusSignal::IsAllGood
        (m_velocity_rot_per_sec, m_statorCurrent, m_supplyCurrent, m_supplyVoltage, m_temperature);
    state.velocity_rad_per_sec = m_velocity_rot_per_sec.GetValueAsDouble() * std::numbers::pi * 2.0;
    state.current_amps = m_supplyCurrent.GetValueAsDouble();
    state.torque_current_amps = m_statorCurrent.GetValueAsDouble();
    state.supply_volts = m_supplyVoltage.GetValueAsDouble();
    state.temperature_celsius = m_temperature.GetValueAsDouble();
    return state;
};
