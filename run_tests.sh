#!/bin/bash

# run_tests.sh
# Build and run the test suite for Tanques-3D physics system
# Usage: ./run_tests.sh [options]
# Options:
#   clean          - Clean build before running
#   --filter=TEST  - Run only tests matching filter (e.g., --filter=Vector*)
#   --verbose      - Verbose test output
#   --list         - List all available tests without running

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${SCRIPT_DIR}/build"
TEST_EXECUTABLE="${BUILD_DIR}/bin/tests/physics_tests"

echo -e "${BLUE}=== Tanques-3D Test Runner ===${NC}"
echo ""

# Parse arguments
CLEAN_BUILD=false
GTEST_FILTER=""
GTEST_ARGS="--gtest_color=yes"
LIST_TESTS=false

for arg in "$@"; do
    case $arg in
        clean)
            CLEAN_BUILD=true
            ;;
        --filter=*)
            GTEST_FILTER="${arg#*=}"
            GTEST_ARGS="${GTEST_ARGS} --gtest_filter=${GTEST_FILTER}"
            ;;
        --verbose)
            GTEST_ARGS="${GTEST_ARGS} --gtest_print_time=1"
            ;;
        --list)
            LIST_TESTS=true
            ;;
        --help)
            echo "Usage: ./run_tests.sh [options]"
            echo ""
            echo "Options:"
            echo "  clean          - Clean build before running tests"
            echo "  --filter=TEST  - Run only tests matching filter (e.g., --filter=Vector*)"
            echo "  --verbose      - Show detailed test timing information"
            echo "  --list         - List all available tests without running them"
            echo "  --help         - Show this help message"
            echo ""
            echo "Examples:"
            echo "  ./run_tests.sh                           # Run all tests"
            echo "  ./run_tests.sh clean                     # Clean build and run all tests"
            echo "  ./run_tests.sh --filter=VectorTest*      # Run only VectorTest suite"
            echo "  ./run_tests.sh --filter=*.Addition       # Run all Addition tests"
            echo "  ./run_tests.sh --list                    # List all tests"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $arg${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Build tests
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Building tests (clean)...${NC}"
    ./build_tests.sh clean
else
    echo -e "${YELLOW}Building tests...${NC}"
    ./build_tests.sh
fi

echo ""

# Check if test executable exists
if [ ! -f "${TEST_EXECUTABLE}" ]; then
    echo -e "${RED}Error: Test executable not found!${NC}"
    echo "Expected location: ${TEST_EXECUTABLE}"
    exit 1
fi

# List tests if requested
if [ "$LIST_TESTS" = true ]; then
    echo -e "${BLUE}Available tests:${NC}"
    "${TEST_EXECUTABLE}" --gtest_list_tests
    exit 0
fi

# Run tests
echo -e "${BLUE}=== Running Tests ===${NC}"
echo ""

if [ -n "$GTEST_FILTER" ]; then
    echo -e "${YELLOW}Filter: ${GTEST_FILTER}${NC}"
    echo ""
fi

# Run the tests and capture exit code
set +e
"${TEST_EXECUTABLE}" ${GTEST_ARGS}
TEST_EXIT_CODE=$?
set -e

echo ""

# Report results
if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}=== All Tests Passed ===${NC}"
else
    echo -e "${RED}=== Some Tests Failed ===${NC}"
    echo ""
    echo "To run only failed tests on next run, use:"
    echo "  ./run_tests.sh --filter=<FailedTestName>"
fi

exit $TEST_EXIT_CODE
