cmake_minimum_required(VERSION 3.14)
project(6502Emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags for debugging
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")

# Include directories
include_directories(src)

# Source files for the emulator library
set(EMULATOR_SOURCES
    src/cpu.cpp
)

# Create a library for the emulator core
add_library(emulator_lib ${EMULATOR_SOURCES})

# Optional: Create executable for standalone use
# add_executable(emulator src/main.cpp)
# target_link_libraries(emulator emulator_lib)

# Google Test setup
# Add GoogleTest subdirectory (already cloned locally)
add_subdirectory(googletest)

# Enable testing
enable_testing()

# Test executable
add_executable(
  emulator_tests
  tests/test_cpu.cpp
  tests/test_main.cpp
)

# Link test executable with Google Test and emulator library
target_link_libraries(
  emulator_tests
  emulator_lib
  gtest_main
  gtest
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(emulator_tests)



