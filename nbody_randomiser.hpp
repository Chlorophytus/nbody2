#ifndef NBODY2_NBODY_RANDOMISER_HPP
#define NBODY2_NBODY_RANDOMISER_HPP

#include "nbody.hpp"

namespace nbody {
    /** nbody::randomiser: Random number factory, generate random real
     *
     * @tparam T : The type of real. Must satisfy `std::is_floating_point` trait.
     */
    template <typename T>
    class randomiser {
        std::unique_ptr<std::random_device> _rd;
        std::unique_ptr<std::uniform_real_distribution<T>> _real_distribution{nullptr};
        static_assert(std::is_floating_point<T>::value, "nbody::randomiser - T isn't a real type");
    public:
        randomiser();
        T shake(T);
    };
}

#endif //NBODY2_NBODY_RANDOMISER_HPP
