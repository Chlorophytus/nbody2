#include "nbody_particle_system.hpp"
using namespace nbody;

static float force;
static std::uint32_t number_of_groups;
static std::uint16_t width;
static std::uint16_t height;
static std::unique_ptr<particle_system::view_t> particles{nullptr};
static std::unique_ptr<particle_system::scratch_t> mutated{nullptr};
static volatile std::size_t semaphore;

bool compare_two_particles(const particle &lhs, const particle &rhs) {
  auto ax = lhs.x;
  auto ay = lhs.y;
  auto amass = lhs.mass;
  auto bx = rhs.x;
  auto by = rhs.y;
  auto bmass = rhs.mass;
  return (std::hypot(ax, ay) * amass) < (std::hypot(bx, by) * bmass);
}

void particle_system::init(
    float _force /**< [in] the force of the particle system. positive repels,
                    negative attracts. */
    ,
    std::uint32_t _number_of_groups /**< [in] higher values are more particles,
                                       and more CPU burn */
    ,
    std::uint16_t _width /**< [in] width to std::fmod all particles */,
    std::uint16_t _height /**< [in] height to std::fmod all particles */) {
  number_of_groups = _number_of_groups;
  width = _width;
  height = _height;
  force = _force;
  particles = std::make_unique<particle_system::view_t>();
  particles->resize(number_of_groups * 9);
  std::for_each(particles->begin(), particles->end(), [](auto &&p) {
    p.reset(new particle{
        .x = randomiser<float>{}.shake(256.0f),
        .dx = randomiser<float>{}.shake(256.0f),
        .y = randomiser<float>{}.shake(256.0f),
        .dy = randomiser<float>{}.shake(256.0f),
        .mass = std::fabs(std::pow(randomiser<float>{}.shake(8.0f), 2.0f))
    });
  });
  mutated = std::make_unique<particle_system::scratch_t>();
  mutated->resize(number_of_groups);
  std::for_each(mutated->begin(), mutated->end(),
                [](auto &&s) { s = shard_t{}; });
}

void particle_system::deinit() {
  mutated = nullptr;
  particles = nullptr;
}

const particle_system::view_t &particle_system::step_and_get() {
  // Would this actually be optimized with the AMD perceptron cache...?
  auto iter = 0;
  auto m = new std::mutex;

  for (auto &&shard : *mutated) {
    shard[0] = *(*particles)[iter + 0];
    shard[1] = *(*particles)[iter + 1];
    shard[2] = *(*particles)[iter + 2];
    shard[3] = *(*particles)[iter + 3];
    shard[4] = *(*particles)[iter + 4];
    shard[5] = *(*particles)[iter + 5];
    shard[6] = *(*particles)[iter + 6];
    shard[7] = *(*particles)[iter + 7];
    shard[8] = *(*particles)[iter + 8];
    semaphore++;
    iter += 9;
  }
  for (auto &&shard : *mutated) {
    std::thread([&shard, &m] {
      std::sort(shard.begin(), shard.end(), compare_two_particles);

      auto leader_x = _mm256_set1_ps(shard[8].x);
      auto leader_y = _mm256_set1_ps(shard[8].y);
      auto leader_mass = _mm256_set1_ps(shard[8].mass);
      auto leader_dx = _mm256_set1_ps(shard[8].dx);
      auto leader_dy = _mm256_set1_ps(shard[8].dy);

      auto ival = 0;

      auto particles_mass = std::array<float, 8>{0.0f};
      auto particles_x = std::array<float, 8>{0.0f};
      auto particles_y = std::array<float, 8>{0.0f};
      auto particles_dx = std::array<float, 8>{0.0f};
      auto particles_dy = std::array<float, 8>{0.0f};
      for (auto &&pval : shard) {
        particles_mass[ival] = pval.mass;
        particles_x[ival] = pval.x;
        particles_y[ival] = pval.y;
        particles_dx[ival] = pval.dx;
        particles_dy[ival] = pval.dy;
        ival++;
      }

      // Pointer arithmetic over here
      auto v_mass = _mm256_loadu_ps(particles_mass.data());
      auto v_x = _mm256_loadu_ps(particles_x.data());
      auto v_y = _mm256_loadu_ps(particles_y.data());
      auto v_dx = _mm256_loadu_ps(particles_dx.data());
      auto v_dy = _mm256_loadu_ps(particles_dy.data());
      auto v_force = _mm256_set1_ps(force);

      // Do the calculations now.
      auto v_ord_x = _mm256_sub_ps(leader_x, v_x);
      auto v_ord_y = _mm256_sub_ps(leader_y, v_y);
			auto v_rp2_x = _mm256_rcp_ps(_mm256_mul_ps(v_ord_x, v_ord_x));
			auto v_rp2_y = _mm256_rcp_ps(_mm256_mul_ps(v_ord_y, v_ord_y));
			auto v_m1m2 = _mm256_mul_ps(v_mass, leader_mass);
      v_dx =
          _mm256_fmadd_ps(v_m1m2, v_rp2_x, v_dx);
      v_dy =
          _mm256_fmadd_ps(v_m1m2, v_rp2_y, v_dy);

      v_x = _mm256_fmadd_ps(v_force, v_x, v_dx);
      v_y = _mm256_fmadd_ps(v_force, v_y, v_dy);
      // Done calculations, have a nice day.

      _mm256_storeu_ps(particles_x.data(), v_x);
      _mm256_storeu_ps(particles_dx.data(), v_dx);
      _mm256_storeu_ps(particles_y.data(), v_y);
      _mm256_storeu_ps(particles_dy.data(), v_dy);

      ival = 0;
      for (auto &&pval : shard) {
        pval.x = std::fmod(particles_x[ival] + static_cast<float>(width >> 1), width) - (width >> 1);
        pval.y = std::fmod(particles_y[ival] + static_cast<float>(height >> 1), height) - (height >> 1);
        pval.dx = particles_dx[ival];
        pval.dy = particles_dy[ival];
        ival++;
      }

      // semaphore here, lock...
      m->lock();
      semaphore--;
      m->unlock();
    }).detach();
  }
  do {
  } while (semaphore > 0);
  iter = 0;
  for (auto &&shard : *mutated) {
    *(*particles)[iter + 0] = shard[0];
    *(*particles)[iter + 1] = shard[1];
    *(*particles)[iter + 2] = shard[2];
    *(*particles)[iter + 3] = shard[3];
    *(*particles)[iter + 4] = shard[4];
    *(*particles)[iter + 5] = shard[5];
    *(*particles)[iter + 6] = shard[6];
    *(*particles)[iter + 7] = shard[7];
    *(*particles)[iter + 8] = shard[8];
    iter += 9;
  }
  delete m;
  return *particles;
}

#if 0
bool particle_system::step(std::size_t stride) {
  if (_particles.at(stride).empty())
    return false;
  float particles_mass[9]{0.0f};
  float particles_x[9]{0.0f};
  float particles_dx[9]{0.0f};
  float particles_y[9]{0.0f};
  float particles_dy[9]{0.0f};

  for (std::uint8_t i = 0; i < 9; i++) {
    while (!_single_file.try_lock())
      ;
    auto j = _particles.at(stride).front().get();
    _single_file.unlock();
    particles_mass[i] = j->mass;
    particles_x[i] = j->x;
    particles_dx[i] = j->dx;
    particles_y[i] = j->y;
    particles_dy[i] = j->dy;
    while (!_single_file.try_lock())
      ;
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

  for (std::uint8_t i = 0; i < 9; i++) {
    while (!_single_file.try_lock())
      ;
    auto j = new particle{fmodf(particles_x[i], static_cast<float>(_w)),
                          particles_dx[i],
                          fmodf(particles_y[i], static_cast<float>(_h)),
                          particles_dy[i], particles_mass[i]};
    _late_particles.emplace(j);

    _single_file.unlock();
  }

  return true;
}

particle_system::particle_system(std::size_t num_particles, std::uint16_t w,
                                 std::uint16_t h)
    : _rand(new randomiser<float>{}), _w(w), _h(h) {
  auto i = num_particles;
  while (i--) {
    auto p = new particle{_rand->shake(static_cast<float>(w) / 4.0f), 0.0f,
                          _rand->shake(static_cast<float>(h) / 4.0f), 0.0f,
                          fabsf(_rand->shake(32.0f))};
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
  for (auto &&i : _early_particles) {
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
  for (auto &&l : _particles) {
    l.sort(compare_two_particles);
  }
  return particle_view;
}


bool particle_system::compare_two_particles(
    const std::unique_ptr<particle> &a, const std::unique_ptr<particle> &b) {
  auto ax = a->x;
  auto ay = a->y;
  auto amass = a->mass;
  auto bx = b->x;
  auto by = b->y;
  auto bmass = b->mass;
	return (hypotf(ax, ay) * amass * force) < (hypotf(bx, by) * bmass * force);
}
#endif
