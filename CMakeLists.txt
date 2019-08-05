cmake_minimum_required(VERSION 3.13)
project(nbody2)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(nbody2 nbody_particle.cpp nbody_particle.hpp
        nbody_randomiser.cpp nbody_randomiser.hpp
        nbody_particle_system.cpp nbody_particle_system.hpp
        nbody.cpp)
target_link_libraries(nbody2 ${SDL2_LIBRARIES})
