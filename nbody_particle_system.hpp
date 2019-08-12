#ifndef NBODY2_NBODY_PARTICLE_SYSTEM_HPP
#define NBODY2_NBODY_PARTICLE_SYSTEM_HPP

#include "nbody.hpp"
#include "nbody_randomiser.hpp"
#include "nbody_particle.hpp"

namespace nbody {
    /** A particle system contains particles and handles making them
     *  - _rand = a randomiser that is dynamic
     *  - _particles = a job queue of particles that need to be rendered
     *  - _late_particles = all particles that have been rendered
     *
     *  - constructor : initialise the particle system with this many particles
     *  - step() : returns if all particle steps have been exhausted
     *  - stir() : redo the particle system, allowing us to step again
     */
    class particle_system {
        std::unique_ptr<randomiser<float>> _rand;
        std::queue<std::unique_ptr<particle>> _late_particles{};
        std::queue<std::unique_ptr<particle>> _particles{};

    public:
        constexpr static float force = 1.0f;
        explicit particle_system(std::size_t);
        bool step();
        std::vector<particle *> stir();
    };
}

#endif //NBODY2_NBODY_PARTICLE_SYSTEM_HPP
