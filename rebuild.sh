#!/bin/bash

set -e

# Usage message
if [[ -z "$1" ]]; then
    echo "Usage:"
    echo "  ./rebuild.sh -build       : Build the project"
    echo "  ./rebuild.sh -watch       : Watch for changes and rebuild"
    echo "  ./rebuild.sh -run         : Run the emulator"
    echo "  ./rebuild.sh -test        : Run tests"
    exit 1
fi

# Build the project
if [[ "$1" == "-build" ]]; then
    cmake -S . -B build -G "Unix Makefiles"
    cmake --build build
    exit 0
fi

# Watch mode (basic loop every 2 seconds)
if [[ "$1" == "-watch" ]]; then
    echo "🔁 Watching for changes every 2 seconds (Ctrl+C to stop)..."
    while true; do
        cmake --build build
        sleep 2
    done
fi

# Run the emulator
if [[ "$1" == "-run" || "$1" == "-test" ]]; then
    if [[ -f build/emulator_tests ]]; then
        ./build/emulator_tests
    else
        echo "❌ build/emulator_tests not found. Did you build?"
    fi
    exit 0
fi

# Invalid option
echo "❌ Invalid option: $1"
exit 1

