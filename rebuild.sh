#!/bin/bash
# rebuild.sh - Linux/macOS build script

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}6502 Emulator Build Script${NC}"
echo -e "${GREEN}================================${NC}"

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -d, --debug     Build in Debug mode"
            echo "  -r, --release   Build in Release mode (default)"
            echo "  -c, --clean     Clean build directory first"
            echo "  -v, --verbose   Verbose build output"
            echo "  -h, --help      Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Build directory
BUILD_DIR="build"

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo -e "${GREEN}Creating build directory...${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo -e "${GREEN}Configuring CMake (${BUILD_TYPE})...${NC}"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      ..

# Build
echo -e "${GREEN}Building...${NC}"
if [ "$VERBOSE" = true ]; then
    cmake --build . --config "$BUILD_TYPE" -- VERBOSE=1
else
    cmake --build . --config "$BUILD_TYPE" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

# Success
echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}================================${NC}"
echo -e "Executable: ${YELLOW}$BUILD_DIR/bin/6502emu${NC}"
echo -e "Library:    ${YELLOW}$BUILD_DIR/lib/libcpu6502.a${NC}"
echo ""
echo -e "To run: ${YELLOW}./$BUILD_DIR/bin/6502emu${NC}"
