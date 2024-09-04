cmake_minimum_required(VERSION 3.29)
project(Parallel_Screensaver)

set(CMAKE_CXX_STANDARD 14)

set(SFML_DIR "C:/SFML/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

#add_executable(Parallel_Screensaver main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(Parallel_Screensaver main_omp.cpp)

target_link_libraries(Parallel_Screensaver sfml-graphics sfml-window sfml-system)
