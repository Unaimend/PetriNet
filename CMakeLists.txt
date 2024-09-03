cmake_minimum_required(VERSION 3.10)

project(Simulator VERSION 0.1)

#set(CMAKE_CXX_CLANG_TIDY clang-tidy)
add_subdirectory(src bin)

include(CTest) 
enable_testing()
add_subdirectory(test)


