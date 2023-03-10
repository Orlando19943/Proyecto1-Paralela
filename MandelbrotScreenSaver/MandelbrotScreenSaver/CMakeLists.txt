# CMakeList.txt : CMake project for MandelbrotScreenSaver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)

# Add source to this project's executable.
add_executable (MandelbrotScreenSaver "MandelbrotScreenSaver.cpp" "MandelbrotScreenSaver.h")

target_link_libraries(MandelbrotScreenSaver SDL2::SDL2 SDL2::SDL2main OpenMP::OpenMP_CXX)

#set(CMAKE_CXX_FLAGS  "${CMAKE_C_FLAGS} /openmp:llvm")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MandelbrotScreenSaver PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
