cmake_minimum_required(VERSION 3.10)

add_library(petriNet STATIC petriNet/src/petriNet.cpp)
target_precompile_headers(petriNet PRIVATE petriNet/include/json.hpp)
target_include_directories(petriNet PRIVATE petriNet/include)

