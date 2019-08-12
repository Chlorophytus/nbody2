#include "nbody_particle_system.hpp"
using namespace nbody;

bool particle_system::step() {
    if(_particles.empty())
        return true;
    // Allocate 16 of each, we will be scattergathering 16 of each affecting 8 of all.
    auto particles_mass = reinterpret_cast<float*>(_mm_malloc(sizeof(float) * 16, 32));
    auto particles_x = reinterpret_cast<float*>(_mm_malloc(sizeof(float) * 16, 32));
    auto particles_dx = reinterpret_cast<float*>(_mm_malloc(sizeof(float) * 16, 32));
    auto particles_y = reinterpret_cast<float*>(_mm_malloc(sizeof(float) * 16, 32));
    auto particles_dy = reinterpret_cast<float*>(_mm_malloc(sizeof(float) * 16, 32));

    for(std::uint8_t i = 0; i < 16; i++) {
        auto j = _particles.front().get();
        particles_mass[i] = j->mass;
        particles_x[i] = j->x;
        particles_dx[i] = j->dx;
        particles_y[i] = j->y;
        particles_dy[i] = j->dy;
        _particles.pop();
    }

    // Pointer arithmetic over here
    auto v_mass_a = _mm256_load_ps(particles_mass + 0);
    auto v_mass_b = _mm256_load_ps(particles_mass + 8);
    auto v_x_a = _mm256_load_ps(particles_x + 0);
    auto v_x_b = _mm256_load_ps(particles_x + 8);
    auto v_dx_a = _mm256_load_ps(particles_dx + 0);
    auto v_dx_b = _mm256_load_ps(particles_dx + 8);
    auto v_y_a = _mm256_load_ps(particles_y + 0);
    auto v_y_b = _mm256_load_ps(particles_y + 8);
    auto v_dy_a = _mm256_load_ps(particles_dy + 0);
    auto v_dy_b = _mm256_load_ps(particles_dy + 8);

    // Do the calculations now.



    // Done calculations, have a nice day.

    _mm256_store_ps(particles_mass + 0, v_mass_a);
    _mm256_store_ps(particles_mass + 8, v_mass_b);
    _mm256_store_ps(particles_x + 0, v_x_a);
    _mm256_store_ps(particles_x + 8, v_x_b);
    _mm256_store_ps(particles_dx + 0, v_dx_a);
    _mm256_store_ps(particles_dx + 8, v_dx_b);
    _mm256_store_ps(particles_y + 0, v_y_a);
    _mm256_store_ps(particles_y + 8, v_y_b);
    _mm256_store_ps(particles_dy + 0, v_dy_a);
    _mm256_store_ps(particles_dy + 8, v_dy_b);

    _mm_free(particles_dy);
    _mm_free(particles_y);
    _mm_free(particles_dx);
    _mm_free(particles_x);
    _mm_free(particles_mass);
    return false;
}

particle_system::particle_system(std::size_t num_particles){
    auto i = num_particles;
    while(--i) {
        auto p = new particle {
                (*_rand)() * 256.0f, 0.0f, (*_rand)() * 256.0f, 0.0f, (*_rand)() + 1.0f * 128.0f
        };
        _particles.emplace(p);
    }
}

std::vector<particle *> particle_system::stir() {
    auto particle_view = std::vector<particle *>();
    while (!_late_particles.empty()) {
        auto i = _late_particles.front().get();
        auto clone = new particle{i->x, i->dx, i->y, i->dy, i->mass};
        _late_particles.pop(); // invalidate it NOW
        _particles.emplace(clone);
        particle_view.push_back(clone);
    }
    return particle_view;
}
