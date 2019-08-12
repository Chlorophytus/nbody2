#ifndef NBODY2_NBODY_PARTICLE_SYSTEM_HPP
#define NBODY2_NBODY_PARTICLE_SYSTEM_HPP

#include "nbody.hpp"
#include "nbody_randomiser.hpp"
#include "nbody_particle.hpp"

namespace nbody {
    /** A particle system contains particles and handles making them
     *  - _rand = a randomiser that is dynamic
     *  - _particles = buckets of particles that need to be rendered.
     *  - _late_particles = all particles that have been rendered
     *
     *  - constructor : initialise the particle system with this many particles
     *  - step() : returns if all particle steps have been exhausted
     *  - stir() : redo the particle system, allowing us to step again
     */
    class particle_system {
        std::unique_ptr<randomiser<float>> _rand{nullptr};
        std::queue<std::unique_ptr<particle>> _late_particles{};
        std::forward_list<std::unique_ptr<particle>> _early_particles; // an initial sort determines which particles go where
        std::vector<std::forward_list<std::unique_ptr<particle>>> _particles{}; // a quicker way to determine if a stack is at its full 16 limit
        std::mutex _single_file;
        std::uint16_t _w, _h;
    public:
        static bool compare_two_particles(const std::unique_ptr<particle> &, const std::unique_ptr<particle> &);
        constexpr static float force = -0.0001f;
        explicit particle_system(std::size_t, std::uint16_t, std::uint16_t);
        bool step(std::size_t);
        std::vector<particle *> stir();
    };
}

#endif //NBODY2_NBODY_PARTICLE_SYSTEM_HPP
