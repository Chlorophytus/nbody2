#include "nbody_framebuffer.hpp"

using namespace nbody;

static std::uint32_t *pixels;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Surface *surface;
static SDL_Texture *texture;
static std::uint16_t w, h;
static std::unique_ptr<particle_system> particles;

void framebuffer::init(std::uint16_t width, std::uint16_t height, int flags, std::size_t num_particles) {
    w = width;
    h = height;
    // Check if pointers aren't null
    assert(SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) == 0);
    assert(window);
    assert(renderer);
    pixels = new std::uint32_t[width * height]{0x00000000};
    surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, 32,
                                       static_cast<int>(sizeof(std::uint32_t) * width),
                                       0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    particles = std::make_unique<particle_system>(num_particles, w, h);
}

void framebuffer::deinit() {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    delete[] pixels;
}

bool framebuffer::poll_and_tick() {
    SDL_Event event;
    SDL_PollEvent(&event);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    auto strides = std::thread::hardware_concurrency();
    volatile auto num_complete = 0;
    for (int i = 0; i < strides; i++) {
        std::thread([i, &num_complete](){
            while(particles->step(i));
            num_complete++;
        }).detach();
    }
    while(num_complete < strides);
    auto particle_view = particles->stir();
    for(std::uint16_t i = 0; i < h; i++) {
        for(std::uint16_t j = 0; j < w; j++) {
            pixels[(i * w) + j] = 0x00000000;
        }
    }
    for(auto &&i : particle_view) {
        auto x = static_cast<std::int16_t>(i->x);
        auto y = static_cast<std::int16_t>(i->y);
        x += w / 2;
        y += h / 2;
        x %= w;
        y %= h;
        pixels[(y * w) + x] = 0xFFFFFFFF;
    }
    SDL_UpdateTexture(texture, nullptr, pixels, static_cast<int>(sizeof(std::uint32_t) * w));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return event.type != SDL_QUIT;
}
