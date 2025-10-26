cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(test)

#add_executable(lab2 main.cpp lab2.cpp lab2.h test/tests.cpp)
add_executable(lab2 main.cpp lab2.cpp lab2.h)

target_link_libraries(lab2 pthread)

#add_definitions(-DTESTS)
