#ifndef NBODY2_NBODY_PARTICLE_SYSTEM_HPP
#define NBODY2_NBODY_PARTICLE_SYSTEM_HPP

#include "nbody.hpp"
#include "nbody_randomiser.hpp"
#include "nbody_particle.hpp"

namespace nbody {
    /** A particle system contains particles and handles making them
     *  - _rand = a randomiser that is dynamic
     *  - _particles = a job queue of particles that need to be rendered
     *
     *  - gather() : gather particles into clumps
     *  - scatter() : scatter particle clumps into particles
     */
    class particle_system {
        std::unique_ptr<randomiser<float>> _rand;
        std::queue<std::unique_ptr<particle>> _particles{};

    public:
        constexpr static float force = 1.0f;

        void gather();

        void scatter(particle_clump &&, particle_clump &&);
    };
}

#endif //NBODY2_NBODY_PARTICLE_SYSTEM_HPP
