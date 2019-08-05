#include "nbody_particle_system.hpp"
using namespace nbody;

void particle_system::gather() {
    std::unique_ptr<particle> particle_grabs0[8]{nullptr};
    std::unique_ptr<particle> particle_grabs1[8]{nullptr};
    for (std::uint8_t i = 0; i < 8; i++) {
        std::swap(particle_grabs0[i], _particles.front());
        _particles.pop();
        std::swap(particle_grabs1[i], _particles.front());
        _particles.pop();

        assert(!_particles.empty());
    }
    // First values
    auto x0 = _mm256_set_ps(particle_grabs0[7]->x, particle_grabs0[6]->x, particle_grabs0[5]->x, particle_grabs0[4]->x,
                           particle_grabs0[3]->x, particle_grabs0[2]->x, particle_grabs0[1]->x, particle_grabs0[0]->x);
    auto y0 = _mm256_set_ps(particle_grabs0[7]->y, particle_grabs0[6]->y, particle_grabs0[5]->y, particle_grabs0[4]->y,
                           particle_grabs0[3]->y, particle_grabs0[2]->y, particle_grabs0[1]->y, particle_grabs0[0]->y);
    auto dx0 = _mm256_set_ps(particle_grabs0[7]->dx, particle_grabs0[6]->dx, particle_grabs0[5]->dx, particle_grabs0[4]->dx,
                           particle_grabs0[3]->dx, particle_grabs0[2]->dx, particle_grabs0[1]->dx, particle_grabs0[0]->dx);
    auto dy0 = _mm256_set_ps(particle_grabs0[7]->dy, particle_grabs0[6]->dy, particle_grabs0[5]->dy, particle_grabs0[4]->dy,
                           particle_grabs0[3]->dy, particle_grabs0[2]->dy, particle_grabs0[1]->dy, particle_grabs0[0]->dy);
    // Then the second ones
    auto x1 = _mm256_set_ps(particle_grabs1[7]->x, particle_grabs1[6]->x, particle_grabs1[5]->x, particle_grabs1[4]->x,
                            particle_grabs1[3]->x, particle_grabs1[2]->x, particle_grabs1[1]->x, particle_grabs1[0]->x);
    auto y1 = _mm256_set_ps(particle_grabs1[7]->y, particle_grabs1[6]->y, particle_grabs1[5]->y, particle_grabs1[4]->y,
                            particle_grabs1[3]->y, particle_grabs1[2]->y, particle_grabs1[1]->y, particle_grabs1[0]->y);
    auto dx1 = _mm256_set_ps(particle_grabs1[7]->dx, particle_grabs1[6]->dx, particle_grabs1[5]->dx, particle_grabs1[4]->dx,
                             particle_grabs1[3]->dx, particle_grabs1[2]->dx, particle_grabs1[1]->dx, particle_grabs1[0]->dx);
    auto dy1 = _mm256_set_ps(particle_grabs1[7]->dy, particle_grabs1[6]->dy, particle_grabs1[5]->dy, particle_grabs1[4]->dy,
                             particle_grabs1[3]->dy, particle_grabs1[2]->dy, particle_grabs1[1]->dy, particle_grabs1[0]->dy);



    auto dm0 = _mm256_set_ps(particle_grabs0[7]->mass, particle_grabs0[6]->mass, particle_grabs0[5]->mass, particle_grabs0[4]->mass,
                             particle_grabs0[3]->mass, particle_grabs0[2]->mass, particle_grabs0[1]->mass, particle_grabs0[0]->mass);

    auto dm1 = _mm256_set_ps(particle_grabs1[7]->mass, particle_grabs1[6]->mass, particle_grabs1[5]->mass, particle_grabs1[4]->mass,
                             particle_grabs1[3]->mass, particle_grabs1[2]->mass, particle_grabs1[1]->mass, particle_grabs1[0]->mass);
    // Gathered, now calculate things
    // t = fma(m0 * m1, recip(distance(t0, t1) * force), t) ???

    // distance(t0, t1) = sqrt((t1*t1)-(t0*t0))
    auto xd = _mm256_sqrt_ps(_mm256_sub_ps(_mm256_mul_ps(x1, x1), _mm256_mul_ps(x0, x0)));
    auto yd = _mm256_sqrt_ps(_mm256_sub_ps(_mm256_mul_ps(y1, y1), _mm256_mul_ps(y0, y0)));
    dx0 = _mm256_fmadd_ps(_mm256_mul_ps(dm0, dm1), _mm256_rcp_ps(_mm256_mul_ps(xd, _mm256_set1_ps(force))), dx0);
    dx1 = _mm256_fmadd_ps(_mm256_mul_ps(dm0, dm1), _mm256_rcp_ps(_mm256_mul_ps(xd, _mm256_set1_ps(force))), dx1);
    dy0 = _mm256_fmadd_ps(_mm256_mul_ps(dm0, dm1), _mm256_rcp_ps(_mm256_mul_ps(yd, _mm256_set1_ps(force))), dy0);
    dy1 = _mm256_fmadd_ps(_mm256_mul_ps(dm0, dm1), _mm256_rcp_ps(_mm256_mul_ps(yd, _mm256_set1_ps(force))), dy1);
    x0 = _mm256_add_ps(x0, dx0);
    x1 = _mm256_add_ps(x1, dx1);
    y0 = _mm256_add_ps(y0, dy0);
    y1 = _mm256_add_ps(y1, dy1);
}

void particle_system::scatter(particle_clump &&_p0, particle_clump &&_p1) {
    auto p0 = _p0;
    auto p1 = _p1;
    auto x0 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto y0 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto dx0 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto dy0 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto x1 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto y1 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto dx1 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto dy1 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto mass0 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    auto mass1 = reinterpret_cast<float *>(_mm_malloc(sizeof(float) * 8, sizeof(char) * 32));
    _mm256_store_ps(x0, p0.x);
    _mm256_store_ps(y0, p0.y);
    _mm256_store_ps(dx0, p0.dx);
    _mm256_store_ps(dy0, p0.dy);
    _mm256_store_ps(x1, p1.x);
    _mm256_store_ps(y1, p1.y);
    _mm256_store_ps(dx1, p1.dx);
    _mm256_store_ps(dy1, p1.dy);
    _mm256_store_ps(mass0, p0.mass);
    _mm256_store_ps(mass1, p1.mass);
    for(std::uint8_t i = 0; i < 8; i++) {
        _particles.emplace(new particle{x0[i], dx0[i], y0[i], dy0[i], mass0[i]});
        _particles.emplace(new particle{x1[i], dx1[i], y1[i], dy1[i], mass1[i]});
    }
    _mm_free(mass1);
    _mm_free(mass0);
    _mm_free(dy1);
    _mm_free(dx1);
    _mm_free(y1);
    _mm_free(x1);
    _mm_free(dy0);
    _mm_free(dx0);
    _mm_free(y0);
    _mm_free(x0);
}