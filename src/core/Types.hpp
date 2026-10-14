#pragma once

// Plain data passed between modules each loop tick:
//   InputState - rider inputs (throttle, brake, cadence) and whether each is valid
//   MotorState - telemetry read back from the motor (speed, current, temperature, supply voltage)
//   Command    - what the motor should do this tick, including whether it may be enabled

enum Mode {
    PID,
    DUTY_CYCLE,
    BRAKE,
    NEUTRAL
};

struct InputState {
    double throttle_vec = 0.0;
    bool throttle_valid = false;
    bool brake = true;
    double cadence_rpm = 0.0;
    bool cadence_valid = false;
};

struct MotorState {
    double velocity_rad_per_sec = 0.0;
    double current_amps = 0.0;
    double supply_volts = 0.0;
    double temperature_celsius = 0.0;
    double torque_current_amps = 0.0;
    bool connected = false;
};

struct Command {
    Mode _mode = Mode::NEUTRAL;
    double duty_cycle = 0.0;
    double setpoint = 0.0;
};