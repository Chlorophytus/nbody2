#ifndef NBODY2_NBODY_PARTICLE_HPP
#define NBODY2_NBODY_PARTICLE_HPP

#include "nbody.hpp"

namespace nbody {
    struct particle {
        float x;
        float dx;
        float y;
        float dy;
        float mass;
    };
    struct particle_clump {
        __m256 x;
        __m256 dx;
        __m256 y;
        __m256 dy;
        __m256 mass;
    };
}

#endif //NBODY2_NBODY_PARTICLE_HPP
