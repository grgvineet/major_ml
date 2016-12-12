cmake_minimum_required(VERSION 3.1)
project(major_ml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Instruct cmake to find the HPX settings
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS system iostreams)

find_package(HPX REQUIRED)

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/src)

# Build library
# On add subdirectory, source file names will be add to $SRCS
set(SRCS ${SRCS})
add_subdirectory(src)
add_library(major ${SRCS})
target_link_libraries(major ${Boost_LIBRARIES} ${ARMADILLO_LIBRARIES})
hpx_setup_target(major)

# Build examples
add_subdirectory(examples)
