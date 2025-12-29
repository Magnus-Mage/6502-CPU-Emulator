cmake_minimum_required(VERSION 3.22)

project(cpu6502_emulator
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Modern C++23 6502 CPU Emulator"
)

# ============================================================================
# Build Configuration
# ============================================================================

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")

# ============================================================================
# Compiler Flags Function (Applied per-target, not globally)
# ============================================================================

function(apply_strict_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4                 # Warning level 4
            /WX                 # Treat warnings as errors
            /permissive-        # Standards conformance mode
            /Zc:__cplusplus     # Enable updated __cplusplus macro
            /utf-8              # Set source and execution character sets to UTF-8
        )
        
        # MSVC-specific optimizations
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target_name} PRIVATE /O2 /Ob2 /Oi /Ot /GL)
            target_link_options(${target_name} PRIVATE /LTCG)
        endif()
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall               # Enable most warnings
            -Wextra             # Enable extra warnings
            -Wpedantic          # Strict ISO C++ compliance
            -Werror             # Treat warnings as errors
            -Wconversion        # Warn on implicit conversions
            -Wsign-conversion   # Warn on sign conversions
            -Wshadow            # Warn on variable shadowing
            -Wnon-virtual-dtor  # Warn on non-virtual destructors
            -Wold-style-cast    # Warn on C-style casts
            -Wcast-align        # Warn on alignment issues
            -Wunused            # Warn on unused variables
            -Woverloaded-virtual # Warn on overloaded virtuals
            -Wnull-dereference  # Warn on null pointer dereferences
            -Wdouble-promotion  # Warn on float to double promotion
        )
        
        # GCC/Clang-specific optimizations
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target_name} PRIVATE -O3 -march=native -flto)
            target_link_options(${target_name} PRIVATE -flto)
        endif()
        
        # Debug flags
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE -g -O0)
        endif()
    endif()
endfunction()

# ============================================================================
# Library: cpu6502 (Static Library)
# ============================================================================

add_library(cpu6502 STATIC
    src/cpu.cpp
)

# Set library properties
set_target_properties(cpu6502 PROPERTIES
    OUTPUT_NAME "cpu6502"
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(cpu6502
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile definitions
target_compile_definitions(cpu6502
    PUBLIC
        $<$<CONFIG:Debug>:CPU6502_DEBUG>
        $<$<CONFIG:Release>:CPU6502_RELEASE>
)

# Apply strict warnings to our library
apply_strict_warnings(cpu6502)

# ============================================================================
# Executable: emulator_demo
# ============================================================================

add_executable(emulator_demo
    src/main.cpp
)

# Set executable properties
set_target_properties(emulator_demo PROPERTIES
    OUTPUT_NAME "6502emu"
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Link with library
target_link_libraries(emulator_demo
    PRIVATE
        cpu6502
)

# Apply strict warnings to our executable
apply_strict_warnings(emulator_demo)

# ============================================================================
# Google Test Setup
# ============================================================================

# Enable testing
enable_testing()

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0  # Latest stable version
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Disable problematic warnings for Google Test (external library)
if(NOT MSVC)
    target_compile_options(gtest PRIVATE -Wno-double-promotion)
    target_compile_options(gtest_main PRIVATE -Wno-double-promotion)
    if(TARGET gmock)
        target_compile_options(gmock PRIVATE -Wno-double-promotion)
    endif()
    if(TARGET gmock_main)
        target_compile_options(gmock_main PRIVATE -Wno-double-promotion)
    endif()
endif()

# ============================================================================
# Test Executables
# ============================================================================

# Test for LDA instructions
add_executable(test_lda
    tests/test_lda.cpp
)

target_link_libraries(test_lda
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_lda)

# Test for ADC instructions
add_executable(test_adc
    tests/test_adc.cpp
)

target_link_libraries(test_adc
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_adc)

# Test for AND instructions
add_executable(test_and
    tests/test_and.cpp
)

target_link_libraries(test_and
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_and)

# Test for ASL instructions
add_executable(test_asl
    tests/test_asl.cpp
)

target_link_libraries(test_asl
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_asl)

# Test for LDX/LDY instructions
add_executable(test_ldxy
    tests/test_ldxy.cpp
)

target_link_libraries(test_ldxy
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_ldxy)

# Test for JSR/RTS instructions
add_executable(test_control_flow
    tests/test_control_flow.cpp
)

target_link_libraries(test_control_flow
    PRIVATE
        cpu6502
        GTest::gtest_main
)

apply_strict_warnings(test_control_flow)

# ============================================================================
# Register Tests with CTest
# ============================================================================

include(GoogleTest)

gtest_discover_tests(test_lda)
gtest_discover_tests(test_adc)
gtest_discover_tests(test_and)
gtest_discover_tests(test_asl)
gtest_discover_tests(test_ldxy)
gtest_discover_tests(test_control_flow)

# ============================================================================
# Test target for running all tests
# ============================================================================

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS 
        test_lda 
        test_adc 
        test_and 
        test_asl 
        test_ldxy
        test_control_flow
    COMMENT "Running all tests..."
)

message(STATUS "==============================================")
message(STATUS "Tests:")
message(STATUS "  - test_lda")
message(STATUS "  - test_adc")
message(STATUS "  - test_and")
message(STATUS "  - test_asl")
message(STATUS "  - test_ldxy")
message(STATUS "  - test_control_flow")
message(STATUS "Run with: make test or make run_tests")
message(STATUS "==============================================")

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

# Install library
install(TARGETS cpu6502
    EXPORT cpu6502Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install executable
install(TARGETS emulator_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install CMake config files
install(EXPORT cpu6502Targets
    FILE cpu6502Targets.cmake
    NAMESPACE cpu6502::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpu6502
)

# Create config file
include(CMakePackageConfigHelpers)

# Simple inline config (no separate .in file needed)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cpu6502Config.cmake" "
# cpu6502Config.cmake
include(CMakeFindDependencyMacro)
include(\"\${CMAKE_CURRENT_LIST_DIR}/cpu6502Targets.cmake\")

set(cpu6502_VERSION ${PROJECT_VERSION})
set(cpu6502_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(cpu6502_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(cpu6502_VERSION_PATCH ${PROJECT_VERSION_PATCH})

if(NOT cpu6502_FIND_QUIETLY)
    message(STATUS \"Found cpu6502: \${cpu6502_VERSION}\")
endif()
")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cpu6502ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cpu6502Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cpu6502ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpu6502
)

# ============================================================================
# Documentation
# ============================================================================

# Install documentation files
install(FILES
    README.md
    TODO.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# ============================================================================
# Build Information
# ============================================================================

message(STATUS "==============================================")
message(STATUS "6502 Emulator Configuration")
message(STATUS "==============================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:      C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:          ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install Prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================================")
message(STATUS "Targets:")
message(STATUS "  - cpu6502 (static library)")
message(STATUS "  - emulator_demo (executable)")
message(STATUS "==============================================")
