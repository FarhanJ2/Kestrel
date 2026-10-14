#include "ControlLoop.hpp"
#include "hw/Motor.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>

int main()
{
    printf("Starting Kestrel...\n");
    try {
        Motor motor{};
        ControlLoop loop{std::chrono::milliseconds{20}};
        loop.Run([&](double dtSeconds) {
            /* read inputs -> controller -> safety -> motor -> log */
        });
    } catch (const std::exception& e) {
        printf("Fatal: %s\n", e.what());
        return EXIT_FAILURE;
    }
    /* command the motor to neutral here before exiting */
    printf("Kestrel stopped\n");
    return EXIT_SUCCESS;
}
