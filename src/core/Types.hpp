#pragma once

// Plain data passed between modules each loop tick:
//   InputState - rider inputs (throttle, brake, cadence) and whether each is valid
//   MotorState - telemetry read back from the motor (speed, current, temperature, supply voltage)
//   Command    - what the motor should do this tick, including whether it may be enabled
