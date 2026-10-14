#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

/**
 * Runs the bike's control loop at a fixed rate.
 *
 * Calls a tick function once per period, tells it how much time really
 * passed since the previous tick, warns when a tick takes longer than
 * the period, and returns cleanly on Ctrl+C or `docker compose stop`.
 *
 * This class only does timing. It knows nothing about the motor or the
 * inputs: reading sensors, running the Controller and Safety, and feeding
 * the motor enable all happen inside the tick function.
 *
 * Typical use from main():
 *
 *     ControlLoop loop{std::chrono::milliseconds{20}};
 *     loop.Run([&](double dtSeconds) {
 *         // read inputs -> controller -> safety -> motor -> log
 *     });
 *     // Run() has returned: command the motor to neutral before exiting
 */
class ControlLoop {
public:
    using Clock = std::chrono::steady_clock;

    /**
     * Called once per period.
     *
     * \param dtSeconds Measured time since the previous tick started.
     *                  Normally equal to the period, larger after an overrun.
     */
    using TickFn = std::function<void(double dtSeconds)>;

    explicit ControlLoop(Clock::duration period);

    /**
     * Runs the loop on the calling thread until Stop() is called or the
     * process receives SIGINT or SIGTERM.
     *
     * The motor is not touched here. It goes neutral on its own once the
     * enable stops being fed, but the caller should still command neutral
     * after this returns.
     */
    void Run(TickFn const &tick);

    /**
     * Requests that the loop exit after the current tick.
     * Safe to call from inside the tick function or from another thread.
     */
    static void Stop();

    /** Number of ticks that took longer than the period since Run() started. */
    uint64_t GetOverrunCount() const { return _overrunCount; }

private:
    static constexpr std::chrono::seconds kOverrunReportInterval{3};

    Clock::duration _period;
    uint64_t _overrunCount = 0;
    uint64_t _unreportedOverruns = 0;
    Clock::time_point _lastOverrunReport{};

    /** Installs the SIGINT/SIGTERM handlers that stop the loop. */
    static void InstallSignalHandlers();

    /** Reports loop overruns with debouncing. */
    void ReportOverrun(Clock::time_point now, Clock::duration measured);
};
