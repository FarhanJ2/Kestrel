#include "ControlLoop.hpp"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <thread>

namespace {
    /* set from the signal handler, so it must be lock-free */
    std::atomic<bool> s_stopRequested{false};
    static_assert(std::atomic<bool>::is_always_lock_free);

    void HandleStopSignal(int)
    {
        s_stopRequested = true;
    }

    double ToMs(ControlLoop::Clock::duration d)
    {
        return std::chrono::duration<double, std::milli>{d}.count();
    }
}

ControlLoop::ControlLoop(Clock::duration period) :
    _period{period}
{}

void ControlLoop::Stop()
{
    s_stopRequested = true;
}

void ControlLoop::InstallSignalHandlers()
{
    struct sigaction action{};
    action.sa_handler = HandleStopSignal;
    sigemptyset(&action.sa_mask);

    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);
}

void ControlLoop::Run(TickFn const &tick)
{
    s_stopRequested = false;
    InstallSignalHandlers();

    _overrunCount = 0;
    _unreportedOverruns = 0;
    _lastOverrunReport = Clock::now();

    printf("Control loop started (%.1fms period)\n", ToMs(_period));

    auto lastStart = Clock::now() - _period;
    auto deadline = Clock::now();

    while (!s_stopRequested) {
        auto const start = Clock::now();
        auto const dt = std::chrono::duration<double>{start - lastStart}.count();
        lastStart = start;

        tick(dt);

        auto const end = Clock::now();
        deadline += _period;

        if (end <= deadline) {
            /* sleep until an absolute time so the loop rate does not drift */
            std::this_thread::sleep_until(deadline);
        } else {
            /* loop overrun; start the next tick now instead of trying to catch up */
            ++_overrunCount;
            ++_unreportedOverruns;
            ReportOverrun(end, end - start);
            deadline = end;
        }
    }

    printf("Control loop stopped\n");
}

void ControlLoop::ReportOverrun(Clock::time_point now, Clock::duration measured)
{
    if (now - _lastOverrunReport > kOverrunReportInterval) {
        fprintf(stderr, "Warning: %.1fms loop time overrun\n"
                "    Last tick took %.3fms, %llu overrun(s) since last report\n",
                ToMs(_period), ToMs(measured),
                static_cast<unsigned long long>(_unreportedOverruns));
        _unreportedOverruns = 0;
        _lastOverrunReport = now;
    }
}
