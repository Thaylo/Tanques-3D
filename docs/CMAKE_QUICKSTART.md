# CMake Quick Start Guide

## TL;DR - Just Build and Run

```bash
# One command to build and run with 15 enemies
./build.sh run 15
```

That's it! The script handles everything automatically.

## First Time Setup

### Install Dependencies

**macOS:**
```bash
# Install CMake (if not already installed)
brew install cmake

# Xcode Command Line Tools (includes OpenGL/GLUT)
xcode-select --install
```

**Ubuntu/Debian Linux:**
```bash
sudo apt-get update
sudo apt-get install cmake build-essential
sudo apt-get install freeglut3-dev libglu1-mesa-dev libgl1-mesa-dev
```

**Fedora/RHEL Linux:**
```bash
sudo dnf install cmake gcc-c++
sudo dnf install freeglut-devel mesa-libGLU-devel
```

## Quick Commands

```bash
# Build the game (Release mode)
./build.sh

# Build and run with default 3 enemies
./build.sh run

# Build and run with 20 enemies
./build.sh run 20

# Clean and rebuild from scratch
./build.sh rebuild

# Build with debug symbols for debugging
./build.sh debug

# Install system-wide to /usr/local/bin
sudo ./build.sh install
```

## Manual CMake Commands

If you prefer to use CMake directly:

```bash
# Configure (run once)
mkdir build && cd build
cmake ..

# Build
cmake --build . --parallel

# Run
./bin/jogoThaylo 15
```

## Build Output Location

After building, find the game at:
```
build/
└── bin/
    ├── jogoThaylo          # The executable
    ├── frente.bmp          # Textures (auto-copied)
    ├── lateralDir.bmp
    ├── lateralEsq.bmp
    ├── sky.bmp
    ├── texture.bmp
    ├── topo.bmp
    └── verso.bmp
```

## Troubleshooting

### "cmake: command not found"
```bash
# macOS
brew install cmake

# Linux
sudo apt-get install cmake  # or sudo dnf install cmake
```

### "OpenGL not found" or "GLUT not found"
```bash
# macOS
xcode-select --install

# Ubuntu/Debian
sudo apt-get install freeglut3-dev libglu1-mesa-dev

# Fedora
sudo dnf install freeglut-devel mesa-libGLU-devel
```

### Build fails or unexpected errors
```bash
# Clean rebuild fixes most issues
./build.sh rebuild
```

### Textures don't load
Make sure you run the game from the build/bin directory or use the full path:
```bash
cd build/bin && ./jogoThaylo 10

# Or
/path/to/build/bin/jogoThaylo 10
```

## Need More Details?

- **User Guide**: See [BUILD_CMAKE.md](BUILD_CMAKE.md) for comprehensive build options
- **Technical Details**: See [CMAKE_IMPLEMENTATION.md](CMAKE_IMPLEMENTATION.md) for implementation details
- **Original Makefile**: Still works! See [Makefile](Makefile) if you prefer the old way

## Why CMake?

- **Cross-platform**: Same commands work on macOS and Linux
- **Parallel builds**: Compiles faster using all CPU cores
- **IDE support**: Works with VSCode, CLion, Xcode
- **Modern**: Industry standard build system
- **Maintainable**: Easier to modify and extend

## Game Controls

Once running:
- **Arrow Keys**: Move tank
- **A/Z**: Increase/decrease turret power
- **Space**: Fire projectile
- **ESC**: Exit game

Enjoy the game!
