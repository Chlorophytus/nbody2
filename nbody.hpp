#ifndef NBODY2_NBODY_HPP
#define NBODY2_NBODY_HPP

/** C Includes
 *  - cassert : need assertions for easy debugging
 *  - cstdint : need standardised types
 *  - cstdio : need a method to print to console
 *  - SDL2 : need something to render with
 *  - x86intrin : need vectoring
 */
#include <SDL2/SDL.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <x86intrin.h>

/** C++ Includes
 *  - initializer_list : makes structs easier
 *  - memory : need *dynamic* allocator that is smart
 *  - new : need errorable allocator
 *  - algorithm : need to sort
 *  - random : need to generate random ordinates
 *  - type_traits : specify traits that we should be using
 *  - vector : for the particleviews
 *  - forward_list : particle groups to pivot
 *  - thread : do threaded stuff
 *  - mutex : single filing the threads
 */
#include <forward_list>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <new>
#include <algorithm>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

/** Other Miscellaneous */

#endif // NBODY2_NBODY_HPP
