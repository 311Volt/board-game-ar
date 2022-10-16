cmake_minimum_required(VERSION 3.0.0)
project(ar-proto VERSION 0.1.0)

find_package(OpenCV)

set(CMAKE_CXX_STANDARD 20)

add_executable(ar-proto main.cpp)

target_link_libraries(ar-proto ${OpenCV_LIBRARIES})

add_custom_command(TARGET ar-proto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/resources
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
