#ifndef NBODY2_NBODY_PARTICLE_SYSTEM_HPP
#define NBODY2_NBODY_PARTICLE_SYSTEM_HPP

#include "nbody.hpp"
#include "nbody_randomiser.hpp"
#include "nbody_particle.hpp"

namespace nbody {
    class particle_system {
        std::unique_ptr<randomiser<float>> _rand;
        std::queue<particle * /* TODO: determine what type of smart pointer to use */> _particles;

    public:
        constexpr static float force = 1.0f;
        void gather();
        void scatter(particle_clump &&, particle_clump &&);
    };
}

#endif //NBODY2_NBODY_PARTICLE_SYSTEM_HPP
