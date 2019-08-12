#include "nbody_randomiser.hpp"
using namespace nbody;

template <typename T> randomiser<T>::randomiser() {
    _real_distribution.reset(new std::uniform_real_distribution<T>{-1.0f, 1.0f});
}

template <typename T> T randomiser<T>::operator()() {
    return (*_real_distribution)(_rd);
}