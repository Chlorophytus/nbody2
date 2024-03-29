#include "nbody_framebuffer.hpp"

using namespace nbody;

static std::uint32_t *pixels;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Surface *surface;
static SDL_Texture *texture;
static std::uint16_t w, h;

void framebuffer::init(std::uint16_t width, std::uint16_t height, int flags,
                       std::uint32_t num_particles) {
  w = width;
  h = height;
  // Check if pointers aren't null
  assert(SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) ==
         0);
  assert(window);
  assert(renderer);
  pixels = new std::uint32_t[width * height]{0x00000000};
  surface =
      SDL_CreateRGBSurfaceFrom(pixels, width, height, 32,
                               static_cast<int>(sizeof(std::uint32_t) * width),
                               0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  particle_system::init(0.005f, num_particles, w, h);
}

void framebuffer::deinit() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_FreeSurface(surface);
  SDL_DestroyWindow(window);
  delete[] pixels;
  particle_system::deinit();
}

bool framebuffer::poll_and_tick() {
  SDL_Event event;
  SDL_PollEvent(&event);
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(renderer);
  auto &&particle_view = particle_system::step_and_get();
  for (std::uint16_t i = 0; i < h; i++) {
    for (std::uint16_t j = 0; j < w; j++) {
      auto oldpixels = pixels[(i * w) + j];
      if (oldpixels == 0x00000000) {
        continue;
      }
      std::uint32_t oldr = (oldpixels & 0xFF000000) >> 24;
      std::uint32_t oldg = (oldpixels & 0x00FF0000) >> 16;
      std::uint32_t oldb = (oldpixels & 0x0000FF00) >> 8;
      oldr = oldr * 0.9f;
      oldg = oldg * 0.9f;
      oldb = oldb * 0.9f;
      pixels[(i * w) + j] =
          (oldr << 24) | (oldg << 16) | (oldb << 8) | 0x000000FF;
    }
  }
  for (auto &&i : particle_view) {
    auto x = static_cast<std::int16_t>(i->x);
    auto y = static_cast<std::int16_t>(i->y);
    x += w / 2;
    y += h / 2;
    x %= w;
    y %= h;
    pixels[(y * w) + x] =
        0xFF0000FF | (static_cast<std::uint8_t>(i->mass * 8.0f) << 8);
  }
  SDL_UpdateTexture(texture, nullptr, pixels,
                    static_cast<int>(sizeof(std::uint32_t) * w));
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
  return (event.type != SDL_QUIT) && (event.key.keysym.sym != SDLK_ESCAPE);
}
