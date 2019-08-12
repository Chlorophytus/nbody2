#include "nbody_particle_system.hpp"
using namespace nbody;

bool particle_system::step(std::size_t stride) {
    if(_particles.at(stride).empty())
        return false;
    float particles_mass[9]{0.0f};
    float particles_x[9]{0.0f};
    float particles_dx[9]{0.0f};
    float particles_y[9]{0.0f};
    float particles_dy[9]{0.0f};

    for(std::uint8_t i = 0; i < 9; i++) {
        while(!_single_file.try_lock());
        auto j = _particles.at(stride).front().get();
        _single_file.unlock();
        particles_mass[i] = j->mass;
        particles_x[i] = j->x;
        particles_dx[i] = j->dx;
        particles_y[i] = j->y;
        particles_dy[i] = j->dy;
        while(!_single_file.try_lock());
        _particles.at(stride).pop_front();
        _single_file.unlock();
    }

    auto leader_x = _mm256_set1_ps(particles_x[0]);
    auto leader_y = _mm256_set1_ps(particles_y[0]);
    auto leader_mass = _mm256_set1_ps(particles_mass[0]);
    auto leader_dx = _mm256_set1_ps(particles_dx[0]);
    auto leader_dy = _mm256_set1_ps(particles_dy[0]);


    // Pointer arithmetic over here
    auto v_mass = _mm256_loadu_ps(particles_mass + 1);
    auto v_x = _mm256_loadu_ps(particles_x + 1);
    auto v_y = _mm256_loadu_ps(particles_y + 1);
    auto v_dx = _mm256_loadu_ps(particles_dx + 1);
    auto v_dy = _mm256_loadu_ps(particles_dy + 1);
    auto v_force = _mm256_set1_ps(force);

    // Do the calculations now.
    auto v_ord_x = _mm256_sub_ps(v_x, leader_x);
    auto v_ord_y = _mm256_sub_ps(v_y, leader_y);
    v_dx = _mm256_fmadd_ps(v_force, _mm256_mul_ps(leader_mass, v_ord_x), v_dx);
    v_dy = _mm256_fmadd_ps(v_force, _mm256_mul_ps(leader_mass, v_ord_y), v_dy);

    v_x = _mm256_add_ps(v_x, v_dx);
    v_y = _mm256_add_ps(v_y, v_dy);

    leader_x = _mm256_add_ps(leader_x, leader_dx);
    leader_y = _mm256_add_ps(leader_y, leader_dy);
    // Done calculations, have a nice day.

    _mm256_storeu_ps(particles_mass + 0, leader_mass);
    _mm256_storeu_ps(particles_x + 0, leader_x);
    _mm256_storeu_ps(particles_dx + 0, leader_dx);
    _mm256_storeu_ps(particles_y + 0, leader_y);
    _mm256_storeu_ps(particles_dy + 0, leader_dy);

    _mm256_storeu_ps(particles_mass + 1, v_mass);
    _mm256_storeu_ps(particles_x + 1, v_x);
    _mm256_storeu_ps(particles_dx + 1, v_dx);
    _mm256_storeu_ps(particles_y + 1, v_y);
    _mm256_storeu_ps(particles_dy + 1, v_dy);

    for(std::uint8_t i = 0; i < 9; i++) {
        while(!_single_file.try_lock());
        auto j = new particle {
                fmodf(particles_x[i], static_cast<float>(_w)),
                particles_dx[i],
                fmodf(particles_y[i], static_cast<float>(_h)),
                particles_dy[i],
                particles_mass[i]
        };
        _late_particles.emplace(j);

        _single_file.unlock();
    }

    return true;
}

particle_system::particle_system(std::size_t num_particles, std::uint16_t w, std::uint16_t h) : _rand(new randomiser<float>{}), _w(w), _h(h) {
    auto i = num_particles;
    while(i--) {
        auto p = new particle {
                _rand->shake(static_cast<float>(w)/4.0f), 0.0f, _rand->shake(static_cast<float>(h)/4.0f),  0.0f, fabsf(_rand->shake(32.0f))
        };
        _late_particles.emplace(p);
    }
    stir();
}

std::vector<particle *> particle_system::stir() {
    auto particle_view = std::vector<particle *>();
    _early_particles.clear();
    while (!_late_particles.empty()) {
        auto i = _late_particles.front().get();
        auto clone = new particle{i->x, i->dx, i->y, i->dy, i->mass};
        _late_particles.pop(); // invalidate it NOW
        _early_particles.emplace_front(clone);
        particle_view.emplace_back(clone);
    }
    auto j = 0;
    for(auto &&i : _early_particles) {
        j++;
    }
    _early_particles.sort(compare_two_particles);
    auto num_threadables = std::thread::hardware_concurrency();
    auto threading = particle_view.size() / num_threadables;
    for (int j = 0; j < num_threadables; j++) {
        _particles.emplace_back();
    }
    // Somehow rank them
    auto counter = 0;
    for (auto &&p : _early_particles) {
        _particles.at(counter).push_front(std::move(p));
        counter++;
        counter %= num_threadables;
    }
    for(auto &&l : _particles) {
        l.sort(compare_two_particles);
    }
    return particle_view;
}

bool particle_system::compare_two_particles(const std::unique_ptr<particle> &a, const std::unique_ptr<particle> &b) {
    auto ax = a->x;
    auto ay = a->y;
    auto amass = a->mass;
    auto bx = b->x;
    auto by = b->y;
    auto bmass = b->mass;
    return (hypotf(ax, ay) * amass * force) < (hypotf(bx, by) * bmass * force);
}
