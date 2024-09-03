# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

#TODO Try out interprocedural optimization

# Specify the build type as Debug
set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -O3)
endif()
# Add an executable target called "baseline"


# Set the target name with the Git commit hash
set(TARGET_NAME "simulator")


add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PUBLIC petriNet)

add_subdirectory(../libs ../libs)

