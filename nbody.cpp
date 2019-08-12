#include "nbody.hpp"
#include "nbody_framebuffer.hpp"

int main(int argc, const char *argv[]) {
    // Do logic. If we can't then assert and die.
    std::fprintf(stdout, "NBody2 v0.1.0 by Roland Metivier\n");

    nbody::framebuffer::init(1600, 900, 0);
    while (nbody::framebuffer::poll_and_tick());
    nbody::framebuffer::deinit();

    return EXIT_SUCCESS;
}