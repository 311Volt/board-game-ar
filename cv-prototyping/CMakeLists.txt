cmake_minimum_required(VERSION 3.0.0)
project(ar-proto VERSION 0.1.0)

find_package(OpenCV CONFIG REQUIRED)
find_package(fmt REQUIRED)

# tesseract
find_package(Tesseract CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_VERBOSE_MAKEFILE 1)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -ffast-math -march=haswell")

file(GLOB_RECURSE AR_PROTO_SOURCES "src/*.cpp")

add_executable(ar-proto ${AR_PROTO_SOURCES})
target_include_directories(ar-proto PRIVATE include/)

# tesseract
include_directories(${Tesseract_INCLUDE_DIRS})
include_directories( ${LEPTONICA_INCLUDE_DIRS} )
link_directories( ${TESSERACT_LIBRARY_DIRS} )
link_directories( ${LEPTONICA_LIBRARY_DIRS} )

# tesseract
target_link_libraries(ar-proto ${OpenCV_LIBRARIES} fmt::fmt  ${Tesseract_LINK_LIBRARIES} ${LEPTONICA_LINK_LIBRARIES})
target_precompile_headers(ar-proto PRIVATE "<opencv2/opencv.hpp>")

add_custom_command(TARGET ar-proto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
