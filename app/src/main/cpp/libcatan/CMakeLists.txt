cmake_minimum_required(VERSION 3.0.0)
project(libcatan VERSION 0.1.0)

find_package(OpenCV CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_VERBOSE_MAKEFILE 1)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -ffast-math -march=haswell")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

file(GLOB_RECURSE AR_PROTO_SOURCES "src/*.cpp")

add_library(libcatan ${AR_PROTO_SOURCES})
target_include_directories(libcatan PUBLIC include/)

target_link_libraries(libcatan ${OpenCV_LIBRARIES})
# target_precompile_headers(libcatan PRIVATE "<catan.hpp>")



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if(NOT LIBCATAN_SKIP_EXAMPLE)
    add_subdirectory(example)
endif()
