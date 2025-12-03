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
# Compiler Flags
# ============================================================================

# Warning flags
if(MSVC)
    add_compile_options(
        /W4                 # Warning level 4
        /WX                 # Treat warnings as errors
        /permissive-        # Standards conformance mode
        /Zc:__cplusplus     # Enable updated __cplusplus macro
        /utf-8              # Set source and execution character sets to UTF-8
    )
    
    # MSVC-specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Ob2 /Oi /Ot /GL)
        add_link_options(/LTCG)
    endif()
else()
    add_compile_options(
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
        add_compile_options(-O3 -march=native -flto)
        add_link_options(-flto)
    endif()
    
    # Debug flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# ============================================================================
# Library: cpu6502 (Static Library)
# ============================================================================

add_library(cpu6502 STATIC
	# src/memory.cpp
    src/cpu.cpp
        # src/cpu_instructions.cpp
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
