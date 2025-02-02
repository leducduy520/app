cmake_minimum_required(VERSION 3.5)
project(HandheldComputer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(magic_enum CONFIG REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets magic_enum::magic_enum common)
target_include_directories(
    ${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/app>
    )
