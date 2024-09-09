cmake_minimum_required(VERSION 3.10)


add_library(petriNet STATIC petriNet/src/petriNet.cpp)
target_precompile_headers(petriNet PRIVATE petriNet/include/json.hpp)
target_include_directories(petriNet PUBLIC petriNet/include)



add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -O0)
if (CMAKE_BUILD_TYPE STREQUAL Debug) 
        target_compile_options(petriNet PRIVATE --coverage) 
        target_link_options(petriNet PUBLIC --coverage) 
        add_custom_command(TARGET petriNet PRE_BUILD COMMAND find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -exec rm {} +)
endif()
