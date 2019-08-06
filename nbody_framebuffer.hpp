#ifndef NBODY2_NBODY_FRAMEBUFFER_HPP
#define NBODY2_NBODY_FRAMEBUFFER_HPP

#include "nbody.hpp"

namespace nbody {
    namespace framebuffer {
        /** A singleton for particle rendering
         *  - init(), deinit() : make and destroy the singleton
         *  - poll_and_tick() : if this returns true then the application should quit
         */
        void init(std::uint16_t, std::uint16_t, int);

        bool poll_and_tick();

        void deinit();
    }
}


#endif //NBODY2_NBODY_FRAMEBUFFER_HPP
