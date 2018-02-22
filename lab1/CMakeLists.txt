cmake_minimum_required(VERSION 3.0)
project(avp-lab1 LANGUAGES C)

set(CMAKE_C_FLAGS_VECTORIZED "-O3 -fopt-info-vec-optimized")
set(CMAKE_C_FLAGS_NOT_VECTORIZED "-O2 -fno-tree-vectorize -fopt-info-vec-optimized")

include_directories(./)
add_executable(lab1 main.c matrix.c)
