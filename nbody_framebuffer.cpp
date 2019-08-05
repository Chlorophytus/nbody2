#include "nbody_framebuffer.hpp"

using namespace nbody;

void framebuffer::init(std::uint16_t width, std::uint16_t height, SDL_WindowFlags flags) {
    // Check if pointers aren't null
    assert(SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) == 0);
    assert(window);
    assert(renderer);
    pixels = new std::uint32_t[width * height]{0x00000000};
    surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, 32,
                                       static_cast<int>(sizeof(std::uint32_t) * width),
                                       0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void framebuffer::deinit() {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    delete[] pixels;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

bool framebuffer::poll_and_tick() {
    SDL_Event event;
    SDL_PollEvent(&event);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    // TODO: Framebuffer should handle scatter-gather queue objects here!
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return event.type != SDL_QUIT;
}
