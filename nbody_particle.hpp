#ifndef NBODY2_NBODY_PARTICLE_HPP
#define NBODY2_NBODY_PARTICLE_HPP

#include "nbody.hpp"

namespace nbody {
    /** A particle, within a system
     *  - x = X ordinate on screen
     *  - dx = a change in the X value
     *  - y = Y ordinate on screen
     *  - dy = a change in the Y value
     *  - mass = how much the particle weighs
     */
    struct particle {
        float x;
        float dx;
        float y;
        float dy;
        float mass;
    };
    /** A particle clump is a vector of 8 particles. */
    struct particle_clump {
        __m256 x;
        __m256 dx;
        __m256 y;
        __m256 dy;
        __m256 mass;
    };
}

#endif //NBODY2_NBODY_PARTICLE_HPP
