cmake_minimum_required(VERSION 3.22.1)
project(Test)

set(CMAKE_C_STANDARD 23)

#add GTK module
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
include_directories(${GTK4_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})