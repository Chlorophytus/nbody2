#include "nbody_randomiser.hpp"
using namespace nbody;

template <> randomiser<float>::randomiser() : _rd(new std::random_device) {
}

template <> float randomiser<float>::shake(float scale) {
    _real_distribution.reset(new std::uniform_real_distribution<float>{-scale, scale});
    return (*_real_distribution)((*_rd));
}