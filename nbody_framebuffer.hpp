#ifndef NBODY2_NBODY_FRAMEBUFFER_HPP
#define NBODY2_NBODY_FRAMEBUFFER_HPP

#include "nbody.hpp"

namespace nbody {
    namespace framebuffer {
        /** A singleton for particle rendering
         *  - pixels = the pixel data
         *  - window, renderer, surface, texture = SDL2 data structures, INTERNAL
         *
         *  - init(), deinit() : make and destroy the singleton
         *  - poll_and_tick() : if this returns true then the application should quit
         */
        std::uint32_t *pixels;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Surface *surface;
        SDL_Texture *texture;

        void init(std::uint16_t, std::uint16_t, SDL_WindowFlags);

        bool poll_and_tick();

        void deinit();
    }
}


#endif //NBODY2_NBODY_FRAMEBUFFER_HPP
