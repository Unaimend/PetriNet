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
  add_compile_options(-pthread -Wall -Wextra -Wpedantic -Wconversion -O0)
endif()
# Add an executable target called "baseline"


# Set the target name with the Git commit hash
set(TARGET_NAME "simulator")


add_library(bootstrap STATIC run.cpp)

if (CMAKE_BUILD_TYPE STREQUAL Debug) 
  message("Compiling in ${CMAKE_BUILD_TYPE} mode")
  target_compile_options(bootstrap PRIVATE --coverage) 
  target_link_options(bootstrap PUBLIC --coverage) 
  add_custom_command(TARGET bootstrap PRE_BUILD COMMAND find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -exec rm {} +)
endif()

find_package(Threads REQUIRED)

add_executable(${TARGET_NAME} main.cpp utils.cpp run.cpp )
target_include_directories(${TARGET_NAME} PUBLIC ../libs/petriNet/include)

target_link_libraries(${TARGET_NAME} PUBLIC bootstrap ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(bootstrap PUBLIC petriNet)

add_subdirectory(../libs ../libs)

