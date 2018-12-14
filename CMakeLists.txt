cmake_minimum_required(VERSION 2.8.4)
project(Objects_detection_sample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp)
add_executable(Objects_detection_sample main.cpp)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( Objects_detection_sample ${OpenCV_LIBS})