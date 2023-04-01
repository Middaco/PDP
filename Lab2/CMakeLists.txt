cmake_minimum_required(VERSION 3.17)
project(Lab2)

set(CMAKE_CXX_STANDARD 20)

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(Lab2 main.cpp mpi_version.cpp)

# Add libraries for code completion and compiling
target_link_libraries(Lab2 PUBLIC MPI::MPI_CXX)