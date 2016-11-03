cmake_minimum_required(VERSION 3.1)
project(major_ml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Instruct cmake to find the HPX settings
find_package(HPX REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

# Build library
# On add subdirectory, source file names will be add to $SRCS
set(SRCS ${SRCS})
add_subdirectory(src)
add_library(major ${SRCS})
hpx_setup_target(major)

# Build examples
add_subdirectory(examples)
