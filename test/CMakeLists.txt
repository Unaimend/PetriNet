cmake_minimum_required(VERSION 3.10)


# AFAIK this should not be necessary, setting it in the main cmakelist file should be enough
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)


if(CMAKE_COMPILER_IS_GNUCXX)
        add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -g -O1)
endif()

add_executable (simple_tests src/simple_tests.cpp)
# Create a Test called MyTest that calls the Test executable created above

# Includes shared with the main application
target_include_directories(simple_tests PRIVATE ${Simulator_SOURCE_DIR}/include)
target_include_directories(simple_tests PRIVATE include)

target_link_libraries(simple_tests PUBLIC petriNet)

add_test (NAME MyTest COMMAND simple_tests)

