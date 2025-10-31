#!/bin/bash

# Build script for Tanques-3D
# Provides easy commands for common build operations

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project settings
PROJECT_NAME="Tanques-3D"
BUILD_DIR="build"
EXECUTABLE="jogoThaylo"

# Helper functions
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    cat << EOF
${GREEN}Tanques-3D Build Script${NC}

Usage: ./build.sh [command] [options]

${YELLOW}Commands:${NC}
  config          Configure CMake (run once or after CMakeLists.txt changes)
  build           Build the project (default: Release)
  rebuild         Clean and rebuild
  clean           Remove build artifacts
  run [enemies]   Build and run the game (default: 3 enemies)
  debug           Configure and build in Debug mode
  release         Configure and build in Release mode
  install         Install the game system-wide (requires sudo)
  test            Build and run with 15 enemies for testing
  help            Show this help message

${YELLOW}Examples:${NC}
  ./build.sh                  # Quick build (release mode)
  ./build.sh run 20           # Build and run with 20 enemies
  ./build.sh debug            # Build with debug symbols
  ./build.sh rebuild          # Clean and rebuild from scratch
  ./build.sh install          # Install to /usr/local/bin

${YELLOW}Requirements:${NC}
  - CMake 3.10 or higher
  - C++ compiler (g++/clang++)
  - OpenGL and GLUT libraries

EOF
}

check_cmake() {
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake 3.10 or higher."
        exit 1
    fi

    CMAKE_VERSION=$(cmake --version | head -n1 | sed 's/cmake version //')
    print_info "Using CMake $CMAKE_VERSION"
}

configure_cmake() {
    local build_type=${1:-Release}

    print_info "Configuring CMake (Build Type: $build_type)..."

    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    cmake -DCMAKE_BUILD_TYPE="$build_type" ..

    cd ..
    print_success "Configuration complete!"
}

build_project() {
    if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/Makefile" ]; then
        print_warning "Build directory not configured. Running configuration..."
        configure_cmake "Release"
    fi

    print_info "Building project..."

    # Detect number of CPU cores for parallel build
    if [[ "$OSTYPE" == "darwin"* ]]; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=$(nproc)
    fi

    cmake --build "$BUILD_DIR" --parallel "$CORES"

    print_success "Build complete!"
    print_info "Executable: $BUILD_DIR/bin/$EXECUTABLE"
}

clean_build() {
    print_info "Cleaning build artifacts..."

    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory removed"
    else
        print_warning "Build directory does not exist"
    fi
}

rebuild_project() {
    clean_build
    configure_cmake "Release"
    build_project
}

run_game() {
    local enemies=${1:-3}

    # Build if needed
    if [ ! -f "$BUILD_DIR/bin/$EXECUTABLE" ]; then
        print_info "Executable not found. Building first..."
        build_project
    fi

    print_info "Running game with $enemies enemies..."
    echo ""

    "$BUILD_DIR/bin/$EXECUTABLE" "$enemies"
}

install_game() {
    if [ ! -f "$BUILD_DIR/bin/$EXECUTABLE" ]; then
        print_error "Build the project first with: ./build.sh build"
        exit 1
    fi

    print_info "Installing $PROJECT_NAME..."

    cd "$BUILD_DIR"
    sudo cmake --install .
    cd ..

    print_success "Installation complete!"
    print_info "You can now run: $EXECUTABLE [num_enemies]"
}

build_debug() {
    clean_build
    configure_cmake "Debug"
    build_project
}

build_release() {
    clean_build
    configure_cmake "Release"
    build_project
}

test_game() {
    print_info "Building and running test configuration..."
    build_project
    run_game 15
}

# Main script logic
main() {
    check_cmake

    local command=${1:-build}

    case "$command" in
        config)
            configure_cmake "Release"
            ;;
        build)
            build_project
            ;;
        rebuild)
            rebuild_project
            ;;
        clean)
            clean_build
            ;;
        run)
            run_game "$2"
            ;;
        debug)
            build_debug
            ;;
        release)
            build_release
            ;;
        install)
            install_game
            ;;
        test)
            test_game
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_error "Unknown command: $command"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
