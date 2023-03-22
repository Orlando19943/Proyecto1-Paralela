cmake_minimum_required(VERSION 3.24)
project(project_01)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)
link_libraries(OpenMP::OpenMP_CXX)

add_executable(parallel01  main.cpp Complex.cpp)
add_executable(parallel02  parallel_01.cpp Complex.cpp)

target_link_libraries(parallel01 SDL2::SDL2main SDL2::SDL2)
target_link_libraries(parallel02 SDL2::SDL2main SDL2::SDL2)

