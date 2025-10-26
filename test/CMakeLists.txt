project(test)

add_subdirectory(gtest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(runTests ../lab2.cpp ../lab2.h tests.cpp)

target_link_libraries(runTests gtest gtest_main)
target_link_libraries(runTests pthread)
