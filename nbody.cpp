#include "nbody.hpp"
#include "nbody_framebuffer.hpp"

int main(int argc, const char *argv[]) {
  // Do logic. If we can't then assert and die.
  auto particle_groups = 36;
  if (argc == 2)
    particle_groups = atoi(argv[1]);
  std::fprintf(stdout, "NBody2 v0.2.0 by Roland Metivier\n");
  std::fprintf(stdout, "...with %d particle groups (%d particles)\n",
               particle_groups, particle_groups * 9);
  nbody::framebuffer::init(1920, 1080, 0, particle_groups);
  auto run = true;
  //  auto iter = std::uint64_t{0};
  do {
    run = nbody::framebuffer::poll_and_tick();
		SDL_Delay(10);
  } while (run);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  nbody::framebuffer::deinit();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return EXIT_SUCCESS;
}
