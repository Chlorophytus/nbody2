#ifndef NBODY2_NBODY_HPP
#define NBODY2_NBODY_HPP

/** C Includes
 *  - cassert : need assertions for easy debugging
 *  - cstdint : need standardised types
 *  - cstdio : need a method to print to console
 *  - SDL2 : need something to render with
 *  - x86intrin : need vectoring
 */
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <SDL2/SDL.h>
#include <x86intrin.h>

/** C++ Includes
 *  - initializer_list : makes structs easier
 *  - memory : need *dynamic* allocator that is smart
 *  - queue : need a queue system from STL, got to organized plop out objects
 *  - random : need to generate random ordinates
 *  - type_traits : specify traits that we should be using
 *  - TODO: threading lib of sorts, by STL
 */
#include <future>
#include <initializer_list>
#include <memory>
#include <queue>
#include <random>
#include <type_traits>

/** Other Miscellaneous */

#endif //NBODY2_NBODY_HPP
