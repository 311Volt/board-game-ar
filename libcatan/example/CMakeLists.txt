cmake_minimum_required(VERSION 3.0.0)
project(libcatan-sample VERSION 1.0)

find_package(fmt REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(libcatan-sample main.cpp)
target_link_libraries(libcatan-sample PUBLIC libcatan fmt::fmt)

add_custom_command(TARGET libcatan-sample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/resources
)
