cmake_minimum_required(VERSION 3.10)

project(main)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Boost 1.78.0 COMPONENTS log REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main src/main.cpp src/LaneDetector.cpp)

target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main Boost::log)
