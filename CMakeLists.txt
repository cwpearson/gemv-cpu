cmake_minimum_required(VERSION 3.0)
project(gemv)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(BENCHMARK_ENABLE_TESTING OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/benchmark)

# Create an executable target
add_executable(main
main.cpp
gemv.cpp)

target_link_libraries(main benchmark::benchmark)
# Set include directories
target_include_directories(main PRIVATE include)
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -ffast-math -march=native)


