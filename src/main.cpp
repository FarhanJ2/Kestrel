#include "ControlLoop.hpp"
#include <chrono>
#include <cstdio>

int main()
{
    printf("Starting Kestrel...\n");

    ControlLoop loop{std::chrono::milliseconds{20}};
    loop.Run([&](double dtSeconds) {
        /* read inputs -> controller -> safety -> motor -> log */
    });

    /* command the motor to neutral here before exiting */
    printf("Kestrel stopped\n");
    return 0;
}
