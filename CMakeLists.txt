cmake_minimum_required(VERSION 3.5)
project(pardis_ml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Instruct cmake to find the HPX settings
find_package(HPX REQUIRED)

set(SOURCE_FILES mandelbrot.cpp)
# build your application using HPX
add_hpx_executable(mandelbrot
        ESSENTIAL
        SOURCES ${SOURCE_FILES}
        COMPONENT_DEPENDENCIES iostreams)
#add_executable(pardis_ml ${SOURCE_FILES})
