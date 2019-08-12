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
}

#endif //NBODY2_NBODY_PARTICLE_HPP
