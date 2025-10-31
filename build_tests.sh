#!/bin/bash

# build_tests.sh
# Build only the test suite for Tanques-3D physics system
# Usage: ./build_tests.sh [clean]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${SCRIPT_DIR}/build"

echo -e "${GREEN}=== Tanques-3D Test Build Script ===${NC}"
echo ""

# Check if clean build requested
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "${BUILD_DIR}"
    echo "Clean complete."
    echo ""
fi

# Create build directory if it doesn't exist
if [ ! -d "${BUILD_DIR}" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir -p "${BUILD_DIR}"
fi

# Navigate to build directory
cd "${BUILD_DIR}"

# Configure with CMake (enable testing)
echo -e "${YELLOW}Configuring CMake with testing enabled...${NC}"
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..

echo ""
echo -e "${YELLOW}Building test suite...${NC}"
# Build only the test target
cmake --build . --target physics_tests -- -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo ""
if [ -f "${BUILD_DIR}/bin/tests/physics_tests" ]; then
    echo -e "${GREEN}=== Build Successful ===${NC}"
    echo -e "Test executable: ${BUILD_DIR}/bin/tests/physics_tests"
    echo ""
    echo "To run tests:"
    echo "  ./run_tests.sh"
    echo ""
    echo "Or run directly:"
    echo "  ${BUILD_DIR}/bin/tests/physics_tests"
    echo ""
else
    echo -e "${RED}=== Build Failed ===${NC}"
    echo "Test executable not found!"
    exit 1
fi
