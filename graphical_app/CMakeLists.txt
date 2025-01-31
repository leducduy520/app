cmake_minimum_required(VERSION 3.5)
project(HandheldComputer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(HandComputer main.cpp)
target_link_libraries(HandComputer Qt5::Widgets)
