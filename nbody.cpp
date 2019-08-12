#include "nbody.hpp"
#include "nbody_framebuffer.hpp"

int main(int argc, const char *argv[]) {
    // Do logic. If we can't then assert and die.
    auto particle_groups = 192;
    particle_groups *= 9;
    particle_groups *= std::thread::hardware_concurrency();
    std::fprintf(stdout, "NBody2 v0.1.0 by Roland Metivier\n");
    std::fprintf(stdout, "...with %d particles\n", particle_groups);
    nbody::framebuffer::init(1600, 900, 0, particle_groups);
    while (nbody::framebuffer::poll_and_tick());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    nbody::framebuffer::deinit();

    return EXIT_SUCCESS;
}