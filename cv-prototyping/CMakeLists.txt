cmake_minimum_required(VERSION 3.0.0)
project(ar-proto VERSION 0.1.0)

find_package(OpenCV)

set(CMAKE_CXX_STANDARD 20)

# dodaje pliki w ten sposob, a nie przez "target_include_directories", bo chce zeby sie zalaczaly w VS, nwm jak ladniej to zrobic XD
set(HEADER_FILES headers/board_coords.hpp
                 headers/board_detection.hpp
                 headers/image_correction.hpp
                 headers/utility_opencv.hpp)

set(SOURCES board_coords.cpp
            board_detection.cpp
            image_correction.cpp
            utility_opencv.cpp)

add_executable(ar-proto main.cpp ${HEADER_FILES} ${SOURCES})
# target_include_directories(ar-proto PRIVATE headers/)

target_link_libraries(ar-proto ${OpenCV_LIBRARIES})

# dodaje resources tez do Debug/, zeby moc uruchamiac bez wlaczania VS plik .exe stamtad
add_custom_command(TARGET ar-proto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/resources
)

add_custom_command(TARGET ar-proto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/resources
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
